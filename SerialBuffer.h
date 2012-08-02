#ifndef SERIAL_BUFFER_H
#define SERIAL_BUFFER_H

#include "Arduino.h"

#define BYTE byte

enum {
    SERIAL_BUFFER_OK        = -1,
    SERIAL_BUFFER_ERROR     = -2
};

class SerialBuffer
{
public:
    static int MESSAGE_START;
    static int MESSAGE_END;
    static int MESSAGE_ESCAPE;
    static int MESSAGE_XOR;
    
    SerialBuffer();
    ~SerialBuffer();
    
    BYTE*       buffer;
    int         bufferSize;
    
    void        reset();
    
    int         startMessage();
    int         write(BYTE b);
    int         endMessage();
    
    int         receive(BYTE b);
    
    int         messageLength() const;
    
private:
    SerialBuffer(const SerialBuffer &other);
    
    int         messageLength_;
    
    enum {
        S_WAIT_START,
        S_IN_MESSAGE,
        S_IN_ESCAPE,
        S_MESSAGE_COMPLETE,
        S_ERROR
    }           receiveState_;
    
};

#endif
