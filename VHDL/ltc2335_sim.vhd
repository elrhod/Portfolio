library ieee;
use     ieee.std_logic_1164.all;
use     ieee.numeric_std.all;

entity ltc2335_sim is 
  port (   
    i_clk_50                : in  std_logic;
    i_reset                 : in  std_logic;
    i_toggle                : in  std_logic;
    i_scki                  : in  std_logic;
    i_sdi                   : in  std_logic;
    i_cnv                   : in  std_logic;
    i_cs_n                  : in  std_logic;
    o_busy                  : out std_logic;
    o_scko                  : out std_logic;
    o_sdo                   : out std_logic;
    o_locked                : out std_logic;
    o_blink                 : out std_logic;
    o_segments0             : out std_logic_vector(6 downto 0);
    o_segments1             : out std_logic_vector(6 downto 0)
  );
end entity ltc2335_sim;

architecture rtl of ltc2335_sim is

component pll is
    port (
      refclk   : in  std_logic := 'X'; -- clk
      rst      : in  std_logic := 'X'; -- reset
      outclk_0 : out std_logic;        -- clk
      locked   : out std_logic         -- export
    );
  end component pll;


  type sequence_array_type is array (0 to 15) of std_logic_vector(7 downto 0);

  signal count_conv : integer range 0 to 511 := 0;
  signal is_conv : std_logic := '0';
  signal count_lim : integer range 0 to 9 := 9;
  signal i_clk_250 : std_logic;
  signal locked    : std_logic;
  signal conv_good : std_logic := '0';
  signal blink_count : unsigned(27 downto 0) := (others=>'0');
  signal blink : std_logic := '0';
  signal sequence_config : sequence_array_type := (("00000111"),("00001111"),("00010111"),("00011111"),("00100111"),("00101111"),("00110111"),("00111111"),
                                                   ("00000000"),("00000000"),("00000000"),("00000000"),("00000000"),("00000000"),("00000000"),("00000000"));
  signal temp_sequence_cntrl : std_logic_vector(7 downto 0);
  signal valid_cntl_byte : std_logic := '0';
  signal cntrl_bit_cnt : integer range 0 to 7 := 0;
  signal clk_count : integer range 0 to 23 := 0;
  signal cntrl_byte_count : integer range 0 to 15 := 0;
  signal save_to_sequencer : std_logic := '0';
  signal input_sequencer_index : integer range 0 to 15 := 0;
  signal output_sequencer_index : integer range 0 to 15 := 0;
  signal current_sequencer_index : integer range 0 to 15 := 0;
  signal output_index_selected : integer range 0 to 7 := 0;
  signal num_elements_in_sequencer : integer range 0 to 15 := 8;
  signal cnv_seen : std_logic := '0';
  signal cs_n_seen : std_logic := '0';

  type ch_array_type is array (0 to 7) of unsigned(15 downto 0);
  signal ch_data : ch_array_type := ((x"0000"),(x"000A"),(x"0014"),(x"001E"),(x"0028"),(x"0032"),(x"003C"),(x"0046"));

  signal cnv_wait_count : integer range 0 to 137 := 0;
  signal cnv_wait_good : std_logic := '0';
  signal cnv_d : std_logic := '0';
  signal temp_sequence_cntrl_250_0 : std_logic_vector (7 downto 0) := (others=>'0');
  signal temp_sequence_cntrl_250_1 : std_logic_vector (7 downto 0) := (others=>'0');
  signal temp_sequence_cntrl_250_2 : std_logic_vector (7 downto 0) := (others=>'0');
  signal save_to_sequencer_250_0 : std_logic := '0';
  signal save_to_sequencer_250_1 : std_logic := '0';
  signal save_to_sequencer_250_2 : std_logic := '0';

  signal sel_ch_data : unsigned(15 downto 0);

  signal digit0 : integer range 0 to 9 := 0;
  signal digit1 : integer range 0 to 9 := 0;


  begin

  pll_inst : component pll
    port map (
      refclk   => i_clk_50,   --  refclk.clk
      rst      => '0',      --   reset.reset
      outclk_0 => i_clk_250, -- outclk0.clk
      locked   => locked    --  locked.export
    );


  o_locked <= locked;
  o_blink <= blink;

  process(i_clk_250)
  begin
    if rising_edge(i_clk_250) then
      if i_reset = '0' then
        count_conv <= 0;
        is_conv <= '0';
        conv_good <= '0';
        blink_count <= (others=>'0');
        blink <= '0';
        ch_data <= ((x"0000"),(x"000A"),(x"0014"),(x"001E"),(x"0028"),(x"0032"),(x"003C"),(x"0046"));
        sequence_config <= (("00000111"),("00001111"),("00010111"),("00011111"),("00100111"),("00101111"),("00110111"),("00111111"),
                           ("00000000"),("00000000"),("00000000"),("00000000"),("00000000"),("00000000"),("00000000"),("00000000"));
        output_sequencer_index <= 0; -- used to lookup sequencer, to select output_index_selected value
        output_index_selected <= 0; -- from sequencer, channel number to output
        cnv_wait_count <= 0;
        cnv_wait_good <= '0';
        o_busy <= '0';
        cnv_d <= '0';
        temp_sequence_cntrl_250_0 <= (others=>'0');
        temp_sequence_cntrl_250_1 <= (others=>'0');
        temp_sequence_cntrl_250_2 <= (others=>'0');
        save_to_sequencer_250_0 <= '0';
        save_to_sequencer_250_1 <= '0';
        save_to_sequencer_250_2 <= '0';
        num_elements_in_sequencer <= 8;
        input_sequencer_index <= 0;
        current_sequencer_index <= 0;
      else

        digit0 <= num_elements_in_sequencer / 10;
        digit1 <= num_elements_in_sequencer - ((num_elements_in_sequencer / 10) * 10);

        save_to_sequencer_250_0 <= save_to_sequencer;
        save_to_sequencer_250_1 <= save_to_sequencer_250_0;
        save_to_sequencer_250_2 <= save_to_sequencer_250_1;

        temp_sequence_cntrl_250_0 <= temp_sequence_cntrl;
        temp_sequence_cntrl_250_1 <= temp_sequence_cntrl_250_0;
        temp_sequence_cntrl_250_2 <= temp_sequence_cntrl_250_1;


        if save_to_sequencer_250_1 = '1' and save_to_sequencer_250_2 = '0' then
          if cnv_seen = '1' or cs_n_seen = '1' then
            num_elements_in_sequencer <= 1;
            input_sequencer_index <= 1;
            cnv_seen <= '0';
            cs_n_seen <= '0';
          else
            if num_elements_in_sequencer < 15 then
              num_elements_in_sequencer <= input_sequencer_index + 1;
              sequence_config(input_sequencer_index) <= temp_sequence_cntrl_250_1;
              input_sequencer_index <= input_sequencer_index + 1;
            elsif (num_elements_in_sequencer = 15) then
              sequence_config(input_sequencer_index) <= temp_sequence_cntrl_250_1;
            end if;
          end if;
        end if;

        cnv_d <= i_cnv;

        blink_count <= blink_count + ("0000000000000000000000000001");

        if blink_count = "1110111001101011001010000000" then
          blink <= not(blink);
          blink_count <= (others=>'0');
        end if;

        if i_toggle = '1' then -- use toggle switch to set whether we wait 40ns or 36 ns
          count_lim <= 9;
        else
          count_lim <= 8;
        end if;

        if i_cs_n = '1' then
          cs_n_seen <= '1';
        end if;


        if i_cnv = '1' then
          cnv_seen <= '1';
          if count_conv < 510 then 
            count_conv <= count_conv + 1;
            if count_conv = 0 then
              conv_good <= '0';
            end if;
          else
            count_conv <= count_conv;
          end if;
          is_conv <= '1';
          o_busy <= '1';
          if cnv_d <= '0' then -- if rising edge of cnv then reset these values
            cnv_wait_count <= 0;
            cnv_wait_good <= '0';
          else -- else increment counter
            if cnv_wait_count < 136 then -- 136 clk periods ~= 550ns
              if i_scki = '1' then -- if clk in starts before 550ns conversion period then we don't consider it a valid wait period
                cnv_wait_count <= 0;
              else
                cnv_wait_count <= cnv_wait_count + 1;
              end if;
            end if;

            if cnv_wait_count = 136 then
              cnv_wait_good <= '1';
              o_busy <= '0';
            else
              cnv_wait_good <= '0';
            end if;
          end if;
        else
          if conv_good = '1' and cnv_wait_count < 136 then -- 136 clk periods ~= 550ns
            if i_scki = '1' then -- if clk in starts before 550ns conversion period then we don't consider it a valid wait period
              cnv_wait_count <= 0;
            else
              cnv_wait_count <= cnv_wait_count + 1;
            end if;
          end if;

          if cnv_wait_count = 136 then
            cnv_wait_good <= '1';
            o_busy <= '0';
          else
            cnv_wait_good <= '0';
          end if;

          count_conv <= 0;
          is_conv <= '0';
          --conv_good <= '0';
        end if;

        if is_conv = '0' then
        else
          if count_conv = count_lim then
            ch_count : for I in 0 to 7 loop
              ch_data(I) <= ch_data(I) + 1;
            end loop ch_count;
            conv_good <= '1';
            if output_sequencer_index < num_elements_in_sequencer-1 then
              output_sequencer_index <= output_sequencer_index + 1;
            else
              output_sequencer_index <= 0;
            end if;
            output_index_selected <= to_integer(unsigned(sequence_config(output_sequencer_index)(5 downto 3)));
            current_sequencer_index <= output_sequencer_index;
          end if;
        end if;

      end if;

    end if;
  end process;

  process(i_scki,is_conv,i_cs_n)
  begin

    if is_conv = '1' or i_cs_n = '1' then
      o_sdo <= ch_data(output_index_selected)(15);
      valid_cntl_byte <= '0';
      cntrl_bit_cnt <= 0;
      clk_count <= 1;
      save_to_sequencer <= '0';
      temp_sequence_cntrl <= (others=>'0');
    elsif i_reset = '0' then
      valid_cntl_byte <= '0';
      cntrl_bit_cnt <= 0;
      clk_count <= 1;
      save_to_sequencer <= '0';

      o_sdo <= 'Z';
      temp_sequence_cntrl <= (others=>'0');
      o_scko <= '0';

    elsif rising_edge(i_scki) then

      if cnv_wait_good = '1' then
        if clk_count <= 15 then
          o_sdo <= ch_data(output_index_selected)(15-clk_count);
          sel_ch_data <= ch_data(output_index_selected);
        else
          o_sdo <= sequence_config(current_sequencer_index)(23-clk_count);
        end if;
      else
        o_sdo <= 'Z';
      end if;

      if cntrl_bit_cnt < 7 then
        cntrl_bit_cnt <= cntrl_bit_cnt + 1;
        save_to_sequencer <= '0';
      else
        if valid_cntl_byte = '1' then
          save_to_sequencer <= '1';
        else
          save_to_sequencer <= '0';
        end if;
        cntrl_bit_cnt <= 0;
      end if;

      if clk_count < 23 then
        clk_count <= clk_count + 1;
      else
        clk_count <= 0;
      end if;

      if cntrl_bit_cnt = 0 then
        if i_sdi = '1' then
          valid_cntl_byte <= '1';
          temp_sequence_cntrl(7) <= '0';
        else
          valid_cntl_byte <= '0';
        end if;
      else
        if valid_cntl_byte = '1' then
          temp_sequence_cntrl(7-cntrl_bit_cnt) <= i_sdi;
        end if;
      end if;



    end if;
  end process;


  ENCODER_PROC0 : process(digit0)
  begin
  case digit0 is
 
    when 0 => o_segments0 <= not("0111111");
    when 1 => o_segments0 <= not("0000110");
    when 2 => o_segments0 <= not("1011011");
    when 3 => o_segments0 <= not("1001111");
    when 4 => o_segments0 <= not("1100110");
    when 5 => o_segments0 <= not("1101101");
    when 6 => o_segments0 <= not("1111101");
    when 7 => o_segments0 <= not("0000111");
    when 8 => o_segments0 <= not("1111111");
    when 9 => o_segments0 <= not("1101111");
   
    end case;
  end process;

  ENCODER_PROC1 : process(digit1)
  begin
  case digit1 is
 
    when 0 => o_segments1 <= not("0111111");
    when 1 => o_segments1 <= not("0000110");
    when 2 => o_segments1 <= not("1011011");
    when 3 => o_segments1 <= not("1001111");
    when 4 => o_segments1 <= not("1100110");
    when 5 => o_segments1 <= not("1101101");
    when 6 => o_segments1 <= not("1111101");
    when 7 => o_segments1 <= not("0000111");
    when 8 => o_segments1 <= not("1111111");
    when 9 => o_segments1 <= not("1101111");
   
    end case;
  end process;

end architecture rtl;