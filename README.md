# ChessBoard_RC522
Chess board with RFID sensors - MFRC522 sensors are located under each cell of the chess board. Chess figures contain RFID tags with the data corresponding to each figure type and color. 

BUTTONS
Upper and middle buttons - scroll.
Lower button - Enter.

WRITING TAGS
Using scroll buttons of the device the color and the type of the figure must be selected. A tag must be placed on A1 cell. After pressing Enter, the display will show OK if the tag was written successfully.

READING TAGS
To start reading process a command 'a' must be sent to the device over UART at 115200 bod.
The device reads all the cells from left to right and from to to bottom, starting with A8.
After the reading is comoplete, the device will send the values of the tags via UART in the same order they were red. After sending the value of a single cell, a space symbol is sent.

FIGURE CODES
Each tag contains a code with the figure type and color that will be red by the device.
    
    
    COLOR is coded in bit 0:
                          0 - black
                          1 - white
                              
    FIGURE ID is coded in bits 1...3:
                          010 - pawn
                          011 - rook
                          100 - knight
                          101 - bishop
                          110 - queen
                          111 - king
    
    CALCULATED WHITE FIGURE CODES:
                          0101 - pawn
                          0111 - rook
                          1001 - knight
                          1011 - bishop
                          1101 - queen
                          1111 - king     
                          
    CALCULATED BLACK FIGURE CODES:
                          0100 - pawn
                          0110 - rook
                          1000 - knight
                          1010 - bishop
                          1100 - queen
                          1110 - king

    ERROR CODE is 0001 - general error

File .ino must be run on Arduino UNO.
File .epro is a project for EasyEDA PRO. It includes CPBs and circuits.

