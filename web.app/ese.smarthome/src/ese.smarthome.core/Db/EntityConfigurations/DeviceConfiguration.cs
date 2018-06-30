using ESE.SmartHome.Core.Devices;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;

namespace ESE.SmartHome.Core.Db.EntityConfigurations
{
    public class DeviceConfiguration : IEntityTypeConfiguration<Device>
    {
        public void Configure(EntityTypeBuilder<Device> builder)
        {
            builder.HasOne(s => s.Setting)
                   .WithOne();

            builder.HasMany(d => d.Measurements)
                   .WithOne();
        }
    }
}