using ESE.SmartHome.Core.Measurements;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;

namespace ESE.SmartHome.Core.Db.EntityConfigurations
{
    public class MeasurementConfiguration : IEntityTypeConfiguration<Measurement>
    {
        public void Configure(EntityTypeBuilder<Measurement> builder)
        {
        }
    }
}