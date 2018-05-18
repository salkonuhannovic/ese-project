using ESE.SmartHome.Core.Devices;
using ESE.SmartHome.Core.Measurements;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;

namespace ESE.SmartHome.Core.Db.EntityConfigurations
{
    public class DeviceConfiguration : IEntityTypeConfiguration<Device>
    {
        public void Configure(EntityTypeBuilder<Device> builder)
        {
            builder.HasMany(d => d.Measurements)
                   .WithOne();
        }
    }
}