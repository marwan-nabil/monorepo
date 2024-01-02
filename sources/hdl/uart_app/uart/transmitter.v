module Transmitter
(
    Enable,
    Clock,
    DataIn,
    DataOut,
    Reset
);
    input Clock;
    input Enable;
    input [7:0] DataIn;
    input Reset;
    output DataOut;

    reg [11:0] Packet;
    reg [3:0] i;
    reg [7:0] data_buffer;
    reg busy_flag,packet_ready_flag;

/*
  initial
  begin
    Packet[11:0] <= 12'b111111111111;
    data_buffer[7:0] <=8'b00000000;
    busy_flag <=1'b0;
    packet_ready_flag <=1'b0;
  end
*/

  assign DataOut = Packet[0];  // the output is the LSB of the Packet shift register
  
always @(posedge Clock or negedge Reset)

  begin
  if (~Reset)
    begin
    Packet[11:0] <= 12'b111111111111;
    data_buffer[7:0] <=8'b00000000;
    busy_flag <=1'b0;
    packet_ready_flag <=1'b0;
    end

 else
 begin

  if (Enable && ~busy_flag)
    begin
      data_buffer[7:0] <= DataIn[7:0];
      busy_flag <= 1'b1;     //insures reading the data bus happens once
    end


  else if (busy_flag && ~packet_ready_flag)
    begin
      Packet <= {2'b11,^data_buffer,data_buffer[7:0],1'b0}; // 2 stop bits and even parity
      packet_ready_flag <= 1'b1;   //insures forming the Packet happens once
    end

  else if (packet_ready_flag && busy_flag)
    for (i=0;i<11;i=i+1)
      begin
        Packet[11] <= 1'b1; // shift in 1's to the MSB
        Packet[i] <= Packet[i+1]; //shifts the whole Packet to the right
      end


  if (Packet[11:0]==12'b111111111111 && packet_ready_flag && busy_flag) // when the Packet is sent and the line is idle
    begin
      packet_ready_flag <=1'b0;
      busy_flag <=1'b0;
    end
  end
  end
endmodule




