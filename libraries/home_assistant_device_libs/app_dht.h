#ifndef _APP_DHT_H_
#define _APP_DHT_H_

#include DHT.h

namespace agg{
    
    class app_dht
    {
        public:
        void dht_init();
        void dht_periodic();
        
        private:
        DHT* dht;
        uint8_t pin;
        uint8_t type;
    };
}

#endif