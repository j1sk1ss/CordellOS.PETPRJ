#include <core/devices/BlockDevice.hpp>

class BIOSDisk : public BlockDevice {
    public:
        BIOSDisk(uint8_t id);

        bool Initialize();
        
        virtual size_t Read(uint8_t* data, size_t size) override;
        virtual size_t Print(const uint8_t* data, size_t size) override;

        virtual bool Seek(SeekPosition pos, int rel) override;

        virtual size_t Size() override;
        virtual size_t Position() override;

    private:
        bool ReadNextSector();
        void LbaToChs(uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut);

        static inline const constexpr int SectorSize = 512;

        uint8_t _Id;

        bool _HaveExtensions;

        uint16_t _Cylinders;
        uint16_t _Sectors;
        uint16_t _Heads;

        uint8_t _Buffer[SectorSize];

        uint64_t _Pos;
        uint64_t _Size;
};