--------------------------------------------------------------------------------
-- Title       : <Title Block>
-- Project     : Default Project Name
--------------------------------------------------------------------------------
-- File        : ltc2325_sim_tb.vhd
-- Author      : Alex McHardy <alex@illusense.com>
-- Company     : illusense inc.
-- Created     : Wed Mar 17 15:07:03 2021
-- Last update : Tue Mar 23 17:25:30 2021
-- Platform    : Default Part Number
-- Standard    : VHDL-2008
--------------------------------------------------------------------------------
-- Copyright (c) 2021 illusense inc.
-------------------------------------------------------------------------------
-- Description: 
--------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use std.textio.all;
use ieee.std_logic_textio.all;

-----------------------------------------------------------

entity ltc2325_sim_tb is

end entity ltc2325_sim_tb;

-----------------------------------------------------------

architecture testbench of ltc2325_sim_tb is

  -- Testbench DUT generics


  -- Testbench DUT ports
  signal i_clk_spi : std_logic;
  signal enable_spi_clk : std_logic :='0';
  signal i_clk_50 : std_logic;
  signal i_conv_n  : std_logic := '1';
  signal o_data0   : std_logic;
  signal o_data1   : std_logic;
  signal o_data2   : std_logic;
  signal o_data3   : std_logic;
  signal o_clk     : std_logic;
  signal reset     : std_logic;
  signal i_toggle  : std_logic:='1';
  signal blink     : std_logic;

  -- Other constants
  constant C_CLK_PERIOD : real := 10.0e-9; -- NS
  constant C_CLK_PERIOD2 : real := 20.0e-9; -- NS

begin
  -----------------------------------------------------------
  -- Clocks and Reset
  -----------------------------------------------------------
  CLK_GEN : process
  begin
    if enable_spi_clk = '1' then
      i_clk_spi <= '1';
      end if;
      wait for C_CLK_PERIOD / 2.0 * (1 SEC);
      i_clk_spi <= '0';
      wait for C_CLK_PERIOD / 2.0 * (1 SEC);
  end process CLK_GEN;

  CLK_GEN2 : process
  begin
    i_clk_50 <= '1';
    wait for C_CLK_PERIOD2 / 2.0 * (1 SEC);
    i_clk_50 <= '0';
    wait for C_CLK_PERIOD2 / 2.0 * (1 SEC);
  end process CLK_GEN2;

  RESET_GEN : process
  begin
    reset <= '0',
             '1' after 10.0*C_CLK_PERIOD * (1 SEC);
    wait;
  end process RESET_GEN;

  -----------------------------------------------------------
  -- Testbench Stimulus
  -----------------------------------------------------------

  -----------------------------------------------------------
  -- Entity Under Test
  -----------------------------------------------------------
  DUT : entity work.ltc2325_sim
    port map (
      i_clk_spi => i_clk_spi,
      i_clk_50  => i_clk_50,
      i_reset   => reset,
      i_conv_n  => i_conv_n,
      i_toggle  => i_toggle,
      o_data0   => o_data0,
      o_data1   => o_data1,
      o_data2   => o_data2,
      o_data3   => o_data3,
      o_clk     => o_clk,
      o_blink   => blink
    );

  process
  begin

      wait until reset = '1';
      i_conv_n <= '1';
      wait for 50 ns;
      i_conv_n <= '0';
      wait for 3 ns;
      enable_spi_clk <= '1';
      wait for 160 ns;
      enable_spi_clk <= '0';
      wait for 200 ns;
      i_conv_n <= '1';
      wait for 30 ns;
      i_conv_n <= '0';
      wait for 3 ns;
      enable_spi_clk <= '1';
      wait for 160 ns;
      enable_spi_clk <= '0';
      wait for 20 ns;
      i_conv_n <= '1';
      wait for 20 ns;
      i_conv_n <= '0';
      wait for 3 ns;
      enable_spi_clk <= '1';
      wait for 160 ns;
      enable_spi_clk <= '0';
      wait for 20 ns;
      i_conv_n <= '1';
      wait for 30 ns;
      i_conv_n <= '0';
      wait for 3 ns;
      enable_spi_clk <= '1';
      wait for 160 ns;
      enable_spi_clk <= '0';
      wait;


  end process;

end architecture testbench;