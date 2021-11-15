#include <stdint.h>
#define SYNCWORD       0xA185
#define END            0x581A
#define VERSION        0x1
#define ENCOID         0x0001
#define CMDPACKET      0x8E
#define REPORTPACKET   0x8F

#define CONTROLBAUDRATE 57600

#define BUFFLEN          128
#define ENCOPACKETLEN    40
#define CONTPACKETLEN    2

struct __attribute__((__packed__)) SAO_data_transport_header 
{
    uint8_t     version;
    uint16_t    packetid;
    uint8_t     message_type;
    uint16_t    packet_counter;
    uint16_t    pdl;
};

struct __attribute__((__packed__)) SAO_data_transport_payload
{
    uint64_t    timestamp[2];
    uint8_t     data[CONTPACKETLEN];
};

struct __attribute__((__packed__)) SAO_data_transport
{
    uint16_t                          syncword;
    struct SAO_data_transport_header  hdr;
    struct SAO_data_transport_payload payload;
    uint16_t                          end;
};