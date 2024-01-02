module Receiver
(
    DataIn,
    DataOut,
    OverSampling,
    Error,
    HostInterrupt,
    HostAcknowledge,
    Reset
);
    input DataIn;
    input OverSampling;
    input HostAcknowledge;
    input Reset;

    output [7:0] DataOut;
    output [2:0] Error;
    output HostInterrupt;

    reg StartFlag;
    reg DataReadyFlag;
    reg HostAcknowledgedFlag;
    reg HostInterrupt;

    reg [11:0] InputRegister;
    reg [3:0] BitCounter;
    reg [3:0] LoopIndex;
    reg [4:0] Counter;
    reg [7:0] BufferRegister;
    reg [2:0] Error;

    assign DataOut = BufferRegister[7:0];

    always @ (negedge Reset)
    begin
        if (~Reset)
        begin
            StartFlag <= 1'b0;
            DataReadyFlag <= 1'b0;
            HostInterrupt <= 1'b0;
            HostAcknowledgedFlag <= 1'b0;
            InputRegister <= 12'b000000000000;
            BitCounter <= 4'b0000;
            LoopIndex <= 4'b0000;
            Counter <= 5'b00000;
            BufferRegister <= 8'b00000000;
            Error <= 3'b000;
        end
    end

    always @ (posedge OverSampling) //first stage,phase locking the OverSampling with the middle of the start bit
    begin
        if ((Counter == 7) && ~StartFlag)
        begin
            StartFlag <= 1'b1; // ensures that the first stage occurs one time only
            InputRegister[11]  <=  DataIn; // start bit entered the shift register
            BitCounter  <=  BitCounter + 1;
            Counter  <= 5'b00000;
        end
        else if (~DataIn && ~StartFlag)
        begin
            Counter <= Counter + 1;
        end
    end

    always @(posedge OverSampling)
    begin
        if (BitCounter == 12)
        begin
            DataReadyFlag <= 1'b1; //this stage would be stuck here until the Counter is Reset by the next stage
        end
        else if (StartFlag && (Counter < 16))
        begin
            Counter <= Counter+1;
        end
        else if (StartFlag && (Counter == 16))
        begin
            for (LoopIndex = 0; LoopIndex < 11; LoopIndex = LoopIndex + 1) //shifts the register to the right
            begin
                InputRegister[LoopIndex] <= InputRegister[LoopIndex + 1];
            end
        end
        
        InputRegister[11] <= DataIn; //shift in the new MSB
        BitCounter <=  BitCounter + 1;
        Counter <= 5'b00000;
    end

    always @(posedge OverSampling)
    begin
        if (DataReadyFlag) //this stage won't start until this flag is set
        begin
            BufferRegister[7:0] <= InputRegister[8:1];
            BitCounter <= 4'b0000; //allowing the previous stage to start again
            StartFlag <= 1'b0;  //allowing the first stage to start again
            HostInterrupt <= 1'b1; //interrupting the host processor to fetch the data word
            DataReadyFlag <= 1'b0; //ensuring this stage occurs only once
            Error[2] <= ^InputRegister[8:1]; //signals a parity error, even parity is considered here
        end
    end

    always@(posedge OverSampling)//data overrun error detection
    begin
        if (HostInterrupt && HostAcknowledgedFlag)
        begin
            HostInterrupt  <=  1'b0;
            HostAcknowledgedFlag  <= 1'b0 ;
        end
        else if (HostInterrupt && DataReadyFlag) // the next data word is ready and the host hasn't aknowledged the previous one
        begin
            Error[0] <= 1'b1; // data overrun ,host failed to fetch in time
        end
    end

    always @ (posedge HostAcknowledge) // the host processor sends a pulse aknowledging the arrival of data
    begin
        HostAcknowledgedFlag <= 1'b1;
    end
endmodule