#include "SerialBuffer.h"

int SerialBuffer::MESSAGE_START     = 0x7F;
int SerialBuffer::MESSAGE_END       = 0x7E;
int SerialBuffer::MESSAGE_ESCAPE    = 0x7D;
int SerialBuffer::MESSAGE_XOR       = 0x20;

#define REMAIN      (bufferSize - messageLength_)
#define APPEND(b)   (buffer[messageLength_++] = (b))

SerialBuffer::SerialBuffer() {}
SerialBuffer::~SerialBuffer() {}

void SerialBuffer::reset()
{
    messageLength_ = 0;
    receiveState_ = S_WAIT_START;
}

int SerialBuffer::startMessage()
{
    reset();
    if (REMAIN) {
        APPEND(MESSAGE_START);
        return 1;
    } else {
        return 0;
    }
}

int SerialBuffer::write(BYTE b)
{
    if (b == MESSAGE_START || b == MESSAGE_END || b == MESSAGE_ESCAPE) {
        if (REMAIN < 2) {
            return 0;
        } else {
            APPEND(MESSAGE_ESCAPE);
            APPEND(MESSAGE_XOR ^ b);
        }
    } else {
        if (REMAIN) {
            APPEND(b);
        } else {
            return 0;
        }
    }
    return 1;
}

int SerialBuffer::endMessage()
{
    if (REMAIN) {
        APPEND(MESSAGE_END);
        return 1;
    } else {
        return 0;
    }
}

int SerialBuffer::receive(BYTE b)
{
    if (b == MESSAGE_START) {
        messageLength_ = 0;
        receiveState_ = S_IN_MESSAGE;
    } else {
        switch (receiveState_) {
            case S_WAIT_START:
                // do nothing, start byte already handled
                break;
            case S_IN_MESSAGE:
                if (b == MESSAGE_ESCAPE) {
                    receiveState_ = S_IN_ESCAPE;
                } else if (b == MESSAGE_END) {
                    receiveState_ = S_WAIT_START;
                    return messageLength_;
                } else {
                    if (REMAIN) {
                        APPEND(b);
                    } else {
                        receiveState_ = S_ERROR;
                    }
                }
                break;
            case S_IN_ESCAPE:
                b ^= MESSAGE_XOR;
                if ((b == MESSAGE_START || b == MESSAGE_END || b == MESSAGE_ESCAPE) && REMAIN) {
                    APPEND(b);
                    receiveState_ = S_IN_MESSAGE;
                } else {
                    receiveState_ = S_ERROR;
                }
            default:
                break;
        }
    }
    return (receiveState_ == S_ERROR)
        ? SERIAL_BUFFER_ERROR
        : SERIAL_BUFFER_OK
        ;
}

int SerialBuffer::messageLength() const
{
    return messageLength_;
}
