extern crate serial;
use serial::prelude::*;

use std::env;
use std::io;
use std::time::Duration;
extern crate prost;
use crc::{Crc, CRC_16_ARC};
use ilx_proto::sysb::{
    Command, CommandName, Error, ErrorCode, FpgaState, GalvoState, NoiseMeasureMode, Waveform,
};
use prost::Message;
use std::io::{stdin, stdout, Read, Write};
pub const CRC16: Crc<u16> = Crc::<u16>::new(&CRC_16_ARC);

pub struct TestCommand<'a> {
    info_msg: &'a str,
    command: Command,
    success_msg: &'a str,
    error_msg: &'a str,
}

impl<'a> TestCommand<'a> {
    fn new(info_msg: &'a str, name: CommandName, success_msg: &'a str, error_msg: &'a str) -> Self {
        let cmd = Command {
            name: Some(name as i32),
            ..Default::default()
        };

        TestCommand {
            info_msg,
            command: cmd,
            success_msg,
            error_msg,
        }
    }

    fn change_command_name(command: &mut Command, name: CommandName) -> &mut Command {
        command.name = Some(name as i32);
        return command;
    }
}

pub trait TestMyCommand {
    // fn new() -> Self;

    fn get_set_fpga_state_cmd(
        dds_pin_value: Option<i32>,
        mode: NoiseMeasureMode,
        noise_trigger_pin_value: Option<i32>,
    ) -> Self;

    fn get_set_galvo_state_cmd(
        x_fov: Option<f32>,
        y_fov: Option<f32>,
        lines: Option<i32>,
        // points: Option<i32>,
        start_scan: Option<bool>,
    ) -> Self;
}

impl TestMyCommand for Command {
    // fn new() -> Self {
    //     Self {
    //         ..Default::default()
    //     }
    // }

    fn get_set_fpga_state_cmd(
        dds_pin_value: Option<i32>,
        mode: NoiseMeasureMode,
        noise_trigger_pin_value: Option<i32>,
    ) -> Self {
        let cmd = Self {
            name: Some(CommandName::FpgaSetState as i32),
            fpga_state: Some(FpgaState {
                dds_pin_value,
                mode: Some(mode as i32),
                noise_trigger_pin_value,
            }),
            ..Default::default()
        };
        return cmd;
    }

    fn get_set_galvo_state_cmd(
        x_fov: Option<f32>,
        y_fov: Option<f32>,
        lines: Option<i32>,
        // points: Option<i32>,
        start_scan: Option<bool>,
    ) -> Self {
        let cmd = Self {
            name: Some(CommandName::GalvoSetState as i32),
            galvo_state: Some(GalvoState {
                ampx: x_fov,
                ampy: y_fov,
                horizontal_sweeps: lines,
                start_scan,
                ..Default::default()
            }),
            ..Default::default()
        };
        return cmd;
    }
}

fn main() {
    for arg in env::args_os().skip(1) {
        println!("connecting to:{:?} \n...", arg);
        let mut port = serial::open(&arg).unwrap();
        interact(&mut port).unwrap();
    }
}

fn interact<T: SerialPort>(port: &mut T) -> io::Result<()> {
    port.reconfigure(&|settings| {
        settings.set_baud_rate(serial::Baud115200)?;
        settings.set_char_size(serial::Bits8);
        settings.set_parity(serial::ParityNone);
        settings.set_stop_bits(serial::Stop1);
        settings.set_flow_control(serial::FlowNone);
        Ok(())
    })?;

    port.set_timeout(Duration::from_millis(1000))?;

    let _res = run_automated_test(port);

    Ok(())
}

fn run_all_commands_test<T: SerialPort>(port: &mut T) -> Result<bool, ()> {
    // get_fw_sha
    let get_sha1_cmd = Command {
        name: Some(CommandName::DbgGetFwSha as i32),
        ..Default::default()
    };

    let reply_rec = send_command(port, get_sha1_cmd);
    // check error message
    assert!(
        reply_rec.clone().ok().unwrap().error.unwrap().code == ErrorCode::Ok as i32,
        "DbgGetFwSha ERROR Received!"
    );
    // check fw_sha size
    assert!(
        reply_rec
            .clone()
            .ok()
            .unwrap()
            .dbg_info
            .unwrap()
            .fw_sha
            .unwrap()
            .chars()
            .count()
            == 7,
        "fw_sha NOT Received!"
    );
    println!("DbgGetFwSha Test Passed!!!");

    pause();

    let mut set_xfov_cmd = Command {
        name: Some(CommandName::FpgaSetState as i32),
        fpga_state: Some(FpgaState {
            mode: Some(NoiseMeasureMode::Manual as i32),
            ..Default::default()
        }),
        ..Default::default()
    };
    let reply_rec = send_command(port, set_xfov_cmd.clone());
    // check error message
    assert!(
        reply_rec.clone().ok().unwrap().error.unwrap().code == ErrorCode::Ok as i32,
        "FpgaSetState noise_trigger_pin_value Error!"
    );
    println!("FpgaSetState NoiseMeasureMode set to Manual Test Passed!!!");
    pause();

    set_xfov_cmd = Command {
        name: Some(CommandName::FpgaSetState as i32),
        fpga_state: Some(FpgaState {
            noise_trigger_pin_value: Some(1),
            ..Default::default()
        }),
        ..Default::default()
    };
    let reply_rec = send_command(port, set_xfov_cmd.clone());
    // check error message
    assert!(
        reply_rec.clone().ok().unwrap().error.unwrap().code == ErrorCode::Ok as i32,
        "FpgaSetState noise_trigger_pin_value Error!"
    );
    println!("FpgaSetState noise_trigger_pin_value Test Passed!!!");
    pause();

    set_xfov_cmd.fpga_state = Some(FpgaState {
        noise_trigger_pin_value: Some(0),
        ..Default::default()
    });
    let reply_rec = send_command(port, set_xfov_cmd.clone());
    // check error message
    assert!(
        reply_rec.clone().ok().unwrap().error.unwrap().code == ErrorCode::Ok as i32,
        "FpgaSetState noise_trigger_pin_value Error!"
    );

    println!("Test Passed!!!");

    Ok(true)
}

fn run_automated_test<T: SerialPort>(port: &mut T) -> Result<bool, ()> {
    let reply_ok_standard = Command {
        error: Some(Error {
            code: ErrorCode::Ok as i32,
            message: Some(String::from("OK")),
        }),
        ..Default::default()
    };

    let set_fpga_noise_auto_cmd = Command {
        name: Some(CommandName::FpgaSetState as i32),
        fpga_state: Some(FpgaState {
            mode: Some(NoiseMeasureMode::Auto as i32),
            ..Default::default()
        }),
        ..Default::default()
    };

    let galvo_cmd1: Command =
        TestMyCommand::get_set_galvo_state_cmd(Some(60.0), Some(60.0), Some(100), Some(true));
    println!("My galvo command: {:#?}", galvo_cmd1);
    let reply_rec = send_command(port, galvo_cmd1.clone());

    assert!(reply_rec.unwrap() == reply_ok_standard, "Errorrrrrr");
    pause();

    let _reply_rec = send_command(port, set_fpga_noise_auto_cmd.clone());
    Ok(true)
}

fn send_command<T: SerialPort>(port: &mut T, command: Command) -> Result<Command, ()> {
    println!("Sending command: {:#?}", command);

    let msg = prost::Message::encode_to_vec(&command);
    println!("msg: {:?}", &msg);
    // let cmd_bytes = msg.encode_to_vec();
    let cmd_len = msg.len();
    let cs = CRC16.checksum(&msg);
    println!("cmd_len: {} cmd_bytes: {:?}", &cmd_len, &msg);

    port.write(&[((cmd_len >> 8) & 0xFF) as u8, (cmd_len & 0xFF) as u8])
        .unwrap();
    port.write(&msg).unwrap();
    port.write(&[((cs >> 8) & 0xFF) as u8, (cs & 0xFF) as u8])
        .unwrap();

    let mut len_buf = vec![0u8; 2];
    let mut msg_buf = vec![0u8; 65536];
    let mut crc_buf = vec![0u8; 2];

    // Wait for reply from sysb and forward to ilx_server.
    let res = port.read_exact(&mut len_buf);
    if res.is_err() {
        println!("[SERIAL]: No reply length!");
    }

    let msg_len = u16::from_be_bytes([len_buf[0], len_buf[1]]) as usize;
    println!("[SERIAL]: Reply len: {}", &msg_len);

    // Read in the specified number of bytes.
    let res = port.read_exact(&mut msg_buf[..msg_len]);
    if res.is_err() {
        println!("[SERIAL]: No reply payload!");
    }
    let msg_slice = &msg_buf[..msg_len];
    println!("[SERIAL]: Reply bytes: {:?}", msg_slice);

    let res = port.read_exact(&mut crc_buf);
    if res.is_err() {
        println!("[SERIAL]: No reply CRC!");
    }
    let cs_ref = u16::from_be_bytes([crc_buf[0], crc_buf[1]]);
    let cs = CRC16.checksum(msg_slice);
    println!("[SERIAL]: Reply Checksums: {:?} / {:?}", &cs, &cs_ref);

    if cs == cs_ref {
        let cmd = Command::decode(msg_slice).unwrap();
        println!("[SERIAL]: Reply: {:#?}", &cmd);
        Ok(cmd)
    } else {
        println!(
            "[SERIAL]: CRC Error on reply (computed = {}, reference = {})",
            cs, cs_ref
        );
        Err(())
    }
}

fn pause() {
    let mut stdout = stdout();
    stdout.write(b"Press Enter to continue...").unwrap();
    stdout.flush().unwrap();
    stdin().read(&mut [0]).unwrap();
}
