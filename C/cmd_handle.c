

  switch(command.name) {
  case CommandName_GALVO_SET_STATE:
    
    reply.error.has_message = 1;

    if (command.galvo_state.has_ampx) {
      if (fabs(command.galvo_state.ampx)/2.0 + fabs(received_params.galvox_field_of_view_offset) > GALVO_MAX_X_FIELD_OF_VIEW/2.0) {
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      } else {
        received_params.galvox_field_of_view = command.galvo_state.ampx;
      }
      Illu_dbg_cmd_msg("CMD:GALVO X FOV CHANGED");
      
    }

    if (command.galvo_state.has_ampy) {
      if (fabs(command.galvo_state.ampy)/2.0 + fabs(received_params.galvoy_field_of_view_offset) > GALVO_MAX_Y_FIELD_OF_VIEW/2.0) {
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      } else {
        received_params.galvoy_field_of_view = command.galvo_state.ampy;
      }
      
      Illu_dbg_cmd_msg("CMD:GALVO Y FOV CHANGED");
    }

    if (command.galvo_state.has_offsetx) {
      if (fabs(command.galvo_state.offsetx) + fabs(received_params.galvox_field_of_view)/2.0 > GALVO_MAX_X_FIELD_OF_VIEW/2.0) {
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      } else {
        received_params.galvox_field_of_view_offset = command.galvo_state.offsetx;
      }
      
      Illu_dbg_cmd_msg("CMD:GALVO X OFFSET CHANGED");
    }

    if (command.galvo_state.has_offsety) {
      if (fabs(command.galvo_state.offsety) + fabs(received_params.galvoy_field_of_view)/2.0 > GALVO_MAX_Y_FIELD_OF_VIEW/2.0) {
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      } else {
        received_params.galvoy_field_of_view_offset = command.galvo_state.offsety;
      }
      
      Illu_dbg_cmd_msg("CMD:GALVO Y OFFSET CHANGED");
    }

    if (command.galvo_state.has_waveformx) {
      switch (command.galvo_state.waveformx) {
        case Waveform_RAMP:
        received_params.x_galvo_waveform = X_GALVO_RAMP;
        break;
        case Waveform_SINE:
        received_params.x_galvo_waveform = X_GALVO_SINE;
        break;
        case Waveform_TRIANGLE:
        received_params.x_galvo_waveform = X_GALVO_TRIANGLE;
        break;
        default:
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      }
      Illu_dbg_cmd_msg("CMD:GALVO X WAVEFORM CHANGED");
    }

    if (command.galvo_state.has_waveformy) {
      switch (command.galvo_state.waveformy) {
        case Waveform_RAMP:
        received_params.y_galvo_waveform = Y_GALVO_RAMP;
        break;
        // case Waveform_SINE:
        ////////////////////////////////// Not Implemented Yet
        // received_params.y_galvo_waveform = Y_GALVO_SINE;
        // break;
        case Waveform_TRIANGLE:
        received_params.y_galvo_waveform = Y_GALVO_TRIANGLE;
        break;

        case Waveform_CUSTOM:
        received_params.y_galvo_waveform = Y_GALVO_CUSTOM;
        if (command.galvo_state.has_y_galvo_distribution_size)  {
          received_params.y_galvo_custom_settings_size = command.galvo_state.y_galvo_distribution_size;
        } else {
          Illu_Set_Error(&reply.error, ErrorCode_MISSING_PARAMETER);
          break;
        }

        for (size_t i = 0; i < received_params.y_galvo_custom_settings_size; i++) {
          for (size_t j = 0; j < 2; j++) {
            received_params.y_galvo_custom_settings[i][j] = command.galvo_state.y_galvo_distribution[i*2+j];
          }
        }
        break;

        default:
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
        break;
      }
      Illu_dbg_cmd_msg("CMD:GALVO Y WAVEFORM CHANGED");
    }

    if (command.galvo_state.has_horizontal_sweeps) {
      if (command.galvo_state.horizontal_sweeps > MAX_LINES_PER_FRAME || command.galvo_state.horizontal_sweeps%2!=0) {
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      } else {
        received_params.lines_per_frame = command.galvo_state.horizontal_sweeps;
      }

      Illu_dbg_cmd_msg("CMD:NUM OF LINES CHANGED");
    }

    if (command.galvo_state.has_galvox_phase_shift) {
      if (command.galvo_state.galvox_phase_shift < 0 || command.galvo_state.galvox_phase_shift > 360) {
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      } else {
        received_params.galvox_phase_shift = command.galvo_state.galvox_phase_shift;
      }

      Illu_dbg_cmd_msg("CMD:GALVO X PHASE SHIFT");
    }

    if (command.galvo_state.has_start_scan) {
      if (command.galvo_state.start_scan) {
        ///////////////
        // Update the parameters with the received Values
        updated_params.lines_per_frame = received_params.lines_per_frame;
        updated_params.points_per_line = received_params.points_per_line;
        updated_params.galvox_field_of_view = received_params.galvox_field_of_view;
        updated_params.galvoy_field_of_view = received_params.galvoy_field_of_view;
        updated_params.galvox_field_of_view_offset = received_params.galvox_field_of_view_offset;
        updated_params.galvoy_field_of_view_offset = received_params.galvoy_field_of_view_offset;
        updated_params.x_galvo_waveform = received_params.x_galvo_waveform;
        updated_params.y_galvo_waveform = received_params.y_galvo_waveform;
        updated_params.galvox_phase_shift = received_params.galvox_phase_shift;

        for (size_t i = 0; i < received_params.y_galvo_custom_settings_size; i++) {
          for (size_t j = 0; j < 2; j++) {
            updated_params.y_galvo_custom_settings[i][j] = received_params.y_galvo_custom_settings[i][j];
          }
        }
        updated_params.y_galvo_custom_settings_size = received_params.y_galvo_custom_settings_size;

        if (first_time) {
          Illu_Run_Configuration(GALVOS);
          Illu_dbg_cmd_msg("CMD:______________START SCAN");
          first_time = 0;
        } else {
          if (is_stopped_flag == 1) {  // If there was a stop command we need to configure everything again
            Illu_Run_Configuration(GALVOS);
            Illu_dbg_cmd_msg("CMD:______________START SCAN");
          } else {  // It was just an update
            update_parameters_flag = 1;
            Illu_dbg_cmd_msg("CMD:______________START SCAN");
          }
        }
      } else {
        Illu_Stop_Configuration(GALVOS);
        Illu_dbg_cmd_msg("CMD:STOP SCAN______________");
      }
    }
    xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
    break;

  case CommandName_UPP_SET_STATE:
    reply.error.has_message = 1;

      if (command.upp_state.has_burst_count) {
        if (command.upp_state.burst_count > MAX_POINTS_PER_LINE || command.upp_state.burst_count < MIN_POINTS_PER_LINE) {
          Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
        } else {
          received_params.points_per_line = command.upp_state.burst_count;
          Illu_dbg_cmd_msg("CMD:POINTS PER LINE CHANGED");
        }
      } else {
        Illu_Set_Error(&reply.error, ErrorCode_MISSING_PARAMETER);
      }
      xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
  	break;

  case CommandName_PHOTO_GET_STATE:
    reply.error.has_message = 1;

    if (!command.photo_diode_state.has_channel) {
      Illu_Set_Error(&reply.error, ErrorCode_MISSING_PARAMETER);
    } else {
      uint8_t seq_adc_receive[3];
      switch (command.photo_diode_state.channel) {
        case PDChannel_OMEGA:
          Illu_Run_Seq_ADC_Direct_Conversion(ADC_SET_CHANNEL6, ADC_INP_RANGE_0V_to_1_25XVREFBUF, seq_adc_receive);
          reply.has_photo_diode_state = 1;  // do I need this???
          reply.photo_diode_state.has_omega_power = 1;
          reply.photo_diode_state.omega_power = (seq_adc_receive[0]<<16)+(seq_adc_receive[1]<<8)+seq_adc_receive[2];
          break;
        case PDChannel_TWOOMEGA:
          Illu_Run_Seq_ADC_Direct_Conversion(ADC_SET_CHANNEL7, ADC_INP_RANGE_0V_to_1_25XVREFBUF, seq_adc_receive);
          reply.has_photo_diode_state = 1;  // do I need this???
          reply.photo_diode_state.has_two_omega_power = 1;
          reply.photo_diode_state.two_omega_power = (seq_adc_receive[0]<<16)+(seq_adc_receive[1]<<8)+seq_adc_receive[2];
          break;
        case PDChannel_DC:
          Illu_Run_Seq_ADC_Direct_Conversion(ADC_SET_CHANNEL5, ADC_INP_RANGE_0V_to_2_5XVREFBUF, seq_adc_receive);
          reply.has_photo_diode_state = 1;  // do I need this???
          reply.photo_diode_state.has_dc_voltage = 1;
          reply.photo_diode_state.dc_voltage = (seq_adc_receive[0]<<16)+(seq_adc_receive[1]<<8)+seq_adc_receive[2];
          break;
      }
    }
    xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
    Illu_dbg_cmd_msg("CMD:PHOTO_GET_STATE");
    break;

  case CommandName_IQ_GET_STATE:
    reply.error.has_message = 1;

    reply.has_iq0_state= 1;  // do I need this???
    for (size_t i = 0; i < num_of_lock_points; i++) {
      reply.iq0_state.iq_mod_data[i] = iq0.res_phase_p->V_out[i];
      reply.iq0_state.iq_mod_data[i+num_of_lock_points] = iq0.res_bias1_p->V_out[i];
      reply.iq0_state.iq_mod_data[i+(2*num_of_lock_points)] = iq0.res_bias2_p->V_out[i];
      reply.iq0_state.iq_mod_data[i+(3*num_of_lock_points)] = iq0.res_phase_p->V_in[i];
      reply.iq0_state.iq_mod_data[i+(4*num_of_lock_points)] = iq0.res_bias1_p->V_in[i];
      reply.iq0_state.iq_mod_data[i+(5*num_of_lock_points)] = iq0.res_bias2_p->V_in[i];
    }
    reply.iq0_state.iq_mod_data_count = 6*num_of_lock_points;
    reply.iq0_state.phase_voltage = iq0.res_phase_p->ind;
    reply.iq0_state.bias1_voltage = iq0.res_bias1_p->ind;
    reply.iq0_state.bias2_voltage = iq0.res_bias2_p->ind;
    reply.iq0_state.has_phase_voltage =1;
    reply.iq0_state.has_bias1_voltage =1;
    reply.iq0_state.has_bias2_voltage =1;

    xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
    Illu_dbg_cmd_msg("CMD:IQ_GET_STATE");
    break;

  case CommandName_IQ_SET_STATE:
    reply.error.has_message = 1;

      if (fabs(command.iq0_state.bias1_voltage) > 8 ||
              fabs(command.iq0_state.bias2_voltage) > 8 ||
              fabs(command.iq0_state.phase_voltage) > 8 ||
              fabs(command.iq1_state.bias1_voltage) > 8 ||
              fabs(command.iq1_state.bias2_voltage) > 8 ||
              fabs(command.iq1_state.phase_voltage) > 8 ) {
          Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      } else {
        switch (ltc2664_span) {
        case DAC_SPAN_CODE_INT_BIP_2_5V_EXT_BIP_VREF:
          convertion_constant =  4095/5.0;
          offset = 2.5;
          break;

        case DAC_SPAN_CODE_INT_0_TO_5V_EXT_0_TO_2VREF:
          convertion_constant =  4095/5.0;
          offset = 0;
          break;
        case DAC_SPAN_CODE_INT_0_TO_10V_EXT_0_TO_4VREF:
          convertion_constant =  4095/10.0;
          offset = 0;
          break;

        case DAC_SPAN_CODE_INT_BIP_5V_EXT_BIP_2VREF:
          convertion_constant =  4095/10.0;
          offset = 5;
          break;

        case DAC_SPAN_CODE_INT_BIP_10V_EXT_BIP_4VREF:
          convertion_constant =  4095/20.0;
          offset = 10;
          break;
        }

        if (command.iq0_state.has_bias1_voltage) {
           output_value = ((uint16_t) ((offset+command.iq0_state.bias1_voltage)*convertion_constant))<<4;
        	hal_ltc2668_spi_write_code(&hspi6, output_value, DAC_DAC14_ADDRESS, DAC_WRITE_REGA);
          hal_ltc2668_spi_update_dac(&hspi6, DAC_DAC14_ADDRESS);
          current_value_bias1 = command.iq0_state.bias1_voltage;
          Illu_dbg_cmd_msg("CMD:IQ_SET_STATE bias1_voltage");
          Illu_dbg_lock_msg("LCK:IQ_SET_STATE bias1_voltage");
          is_lock_coarse =1;
        }
        if (command.iq0_state.has_bias2_voltage) {
          output_value = ((uint16_t) ((offset+command.iq0_state.bias2_voltage)*convertion_constant))<<4;
          hal_ltc2668_spi_write_code(&hspi6, output_value, DAC_DAC13_ADDRESS, DAC_WRITE_REGA);
          hal_ltc2668_spi_update_dac(&hspi6, DAC_DAC13_ADDRESS);
          current_value_bias2 = command.iq0_state.bias2_voltage;
          Illu_dbg_cmd_msg("CMD:IQ_SET_STATE bias2_voltage");
          Illu_dbg_lock_msg("LCK:IQ_SET_STATE bias2_voltage");
          is_lock_coarse =1;
        }
        if (command.iq0_state.has_phase_voltage) {
          output_value = ((uint16_t) ((offset+command.iq0_state.phase_voltage)*convertion_constant))<<4;
          hal_ltc2668_spi_write_code(&hspi6, output_value, DAC_DAC12_ADDRESS, DAC_WRITE_REGA);
          hal_ltc2668_spi_update_dac(&hspi6, DAC_DAC12_ADDRESS);
          current_value_phase = command.iq0_state.phase_voltage;
          Illu_dbg_cmd_msg("CMD:IQ_SET_STATE phase_voltage");
          Illu_dbg_lock_msg("LCK:IQ_SET_STATE phase_voltage");
          is_lock_coarse =1;
        }
      }
      xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
      if (command.iq0_state.has_run) {
        if (is_lock_coarse==1) {
          Illu_dbg_lock_msg("LCK:RUNNING LOCK COARSE");
        } else {
          Illu_dbg_lock_msg("LCK:RUNNING LOCK FINE");
        }
        run_iq_modulator_lock(&iq0);
        if (is_lock_coarse==1) {
          Illu_dbg_lock_msg("LCK:COARSE LOCKING DONE");
        } else {
          Illu_dbg_lock_msg("LCK:FINE LOCKING DONE");
        }
        is_lock_coarse =0;
      }
    break;

  case CommandName_EDFA_SET_STATE:
    reply.error.has_message = 1;

    if (!command.has_edfa_state) {
        Illu_Set_Error(&reply.error, ErrorCode_MISSING_PARAMETER);
    }
    user_EDFA_current = command.edfa_state.current;
    Illu_set_EDFA_Current(user_EDFA_current);
    xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
    Illu_dbg_cmd_msg("CMD:EDFA_SET_STATE");
  	break;

  case CommandName_DBG_GET_FW_SHA:
    reply.error.has_message = 1;

    reply.name = CommandName_DBG_GET_FW_SHA;
    reply.has_dbg_info = 1;
    reply.dbg_info.has_fw_sha = 1;
    for (size_t i = 0; i < 7; i++) {
      reply.dbg_info.fw_sha[i] = sha[i];
    }

    xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
    Illu_dbg_cmd_msg("CMD:DBG_GET_FW_SHA");
  	break;

  case CommandName_FPGA_SET_STATE:
    Illu_dbg_cmd_msg("CMD:FPGA_SET_STATE");

    reply.name = CommandName_FPGA_SET_STATE;
    if (!command.has_fpga_state) {
      Illu_Set_Error(&reply.error, ErrorCode_MISSING_PARAMETER);
    } else {
      if ((command.fpga_state.dds_pin_value == 1) || (command.fpga_state.dds_pin_value == 0)) {
        if ((command.fpga_state.dds_pin_value == 1)) {
          HAL_GPIO_WritePin(DDS_GPIO_Port, DDS_Pin, GPIO_PIN_SET);
          Illu_dbg_cmd_msg("CMD:FPGA_DDS_PIN_VAL = 1");
        } else {
          HAL_GPIO_WritePin(DDS_GPIO_Port, DDS_Pin, GPIO_PIN_RESET);
          Illu_dbg_cmd_msg("CMD:FPGA_DDS_PIN_VAL = 0");
        }
      } else {
        Illu_Set_Error(&reply.error, ErrorCode_OUT_OF_RANGE);
      }
      if (command.fpga_state.has_mode == 1) {
        switch (command.fpga_state.mode) {
          case NoiseMeasureMode_DISABLED:
            HAL_GPIO_WritePin(FPGA_NOISE_TRIGGER_GPIO_Port, FPGA_NOISE_TRIGGER_Pin, GPIO_PIN_RESET);
            fpga_noise_meas_trig_mode = NoiseMeasureMode_DISABLED;
            Illu_dbg_cmd_msg("CMD:NoiseMeasureMode_DISABLED");
          break;

          case NoiseMeasureMode_AUTO:
            fpga_noise_meas_trig_mode = NoiseMeasureMode_AUTO;
            Illu_dbg_cmd_msg("CMD:NoiseMeasureMode_AUTO");
          break;

          case NoiseMeasureMode_MANUAL:
            fpga_noise_meas_trig_mode = NoiseMeasureMode_MANUAL;
            Illu_dbg_cmd_msg("CMD:NoiseMeasureMode_MANUAL");
          break;

          default:
            HAL_GPIO_WritePin(FPGA_NOISE_TRIGGER_GPIO_Port, FPGA_NOISE_TRIGGER_Pin, GPIO_PIN_RESET);
            fpga_noise_meas_trig_mode = NoiseMeasureMode_DISABLED;
            strcpy(reply.error.message, "default MISSING_PARAMETER=fpga_state.mode");
            reply.error.has_message = 1;

            Illu_Set_Error(&reply.error, ErrorCode_MISSING_PARAMETER);
            xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
            Illu_dbg_cmd_msg("CMD:MISSING_PARAMETER=fpga_state.mode");
          break;
        }
      }
      if (command.fpga_state.has_noise_trigger_pin_value==1) {
        if (fpga_noise_meas_trig_mode == NoiseMeasureMode_MANUAL) {
          if (command.fpga_state.noise_trigger_pin_value==1) {
            HAL_GPIO_WritePin(FPGA_NOISE_TRIGGER_GPIO_Port, FPGA_NOISE_TRIGGER_Pin, GPIO_PIN_SET);
            for (size_t i = 0; i < 120; i++) { ;  }
            HAL_GPIO_WritePin(FPGA_NOISE_TRIGGER_GPIO_Port, FPGA_NOISE_TRIGGER_Pin, GPIO_PIN_RESET);
            Illu_dbg_cmd_msg("CMD:FPGA_NOISE_TRIGGER = 1");
          }
        }
      }
    }
    xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
  	break;

  default:
    strcpy(reply.error.message, "default MISSING_PARAMETER");
    reply.error.has_message = 1;

    Illu_Set_Error(&reply.error, ErrorCode_MISSING_PARAMETER);
    xmit_proto_buff(&reply, Com3TxBuffer, TXBUFFERSIZE, huart);
    Illu_dbg_cmd_msg("CMD:MISSING_PARAMETER");
    break;
  }
