module UART_top (reset_tx_rx,baud,inputDataBus,outputDataBus,err,Rx,Tx,clk,host_interrupt,send,host_aknowledged,ticker);
  input Rx,send,clk,host_aknowledged,reset_tx_rx;
  input [1:0] baud;
  input [7:0] inputDataBus;
  
  output [7:0] outputDataBus;
  output host_interrupt,Tx,ticker;
  output [2:0] err;
    
  
  //Baud generator connection
  wire ticker,overSampler;
  baud_generator gen (.clock(clk),.baud_rate(baud),.tick(ticker),.s_tick(overSampler));
  
  //Tx connection
  transmitter trans (.enable(send),.clk(ticker),.dataIn(inputDataBus),.dataOut(Tx),.reset(reset_tx_rx));
  
  //Rx connection
  receiver rec (.dataIn(Rx),.dataOut(outputDataBus),.overSampler(overSampler),.err(err),.host_interrupt(host_interrupt),.host_aknowledged(host_aknowledged),.reset(reset_tx_rx));
endmodule