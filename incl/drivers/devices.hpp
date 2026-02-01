#include <stdint.h>


namespace nosok {
    namespace devices {
        class Device {
            public:
                Device();
                ~Device();
        };

        void init();
        void register_device(Device dev);
    }
}