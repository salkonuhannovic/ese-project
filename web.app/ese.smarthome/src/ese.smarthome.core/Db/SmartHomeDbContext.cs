using ESE.SmartHome.Core.Db.EntityConfigurations;
using ESE.SmartHome.Core.Devices;
using ESE.SmartHome.Core.Measurements;
using JetBrains.Annotations;
using Microsoft.EntityFrameworkCore;

namespace ESE.SmartHome.Core.Db
{
    [UsedImplicitly]
    public class SmartHomeDbContext : DbContext
    {
        public DbSet<Device> Devices { get; set; }
        public DbSet<Measurement> Measurements { get; set; }

        public SmartHomeDbContext(DbContextOptions options) : base(options)
        {

        }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.ApplyConfiguration(new DeviceConfiguration());
            modelBuilder.ApplyConfiguration(new MeasurementConfiguration());
        }
    }
}