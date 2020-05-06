#define MAX_DATA 512


struct rq_packet
{
    uint16_t opcode;
    char data[MAX_DATA + 2];
};

struct data_packet
{
    uint16_t opcode;
    uint16_t block_nr;
    char *data;
};

struct ack_packet
{
    uint16_t opcode; 
    uint16_t block_nr;
};

struct error_packet
{
    uint16_t opcode;
    uint16_t error_code;
    char error_message[MAX_DATA];
    uint8_t zero;
};