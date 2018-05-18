using ESE.SmartHome.Core.Db;
using ESE.SmartHome.Core.Devices;
using ESE.SmartHome.Core.Measurements;
using JetBrains.Annotations;

namespace ESE.SmartHome.Core.Data
{
    public interface ISmartHomeUnitOfWork
    {
        IDeviceRepository Devices { get; }
        IMeasurementRepository Measurements { get; }

        void Save();
    }

    [UsedImplicitly]
    public class SmartHomeUnitOfWork : ISmartHomeUnitOfWork
    {
        private readonly SmartHomeDbContext _dbContext;

        public IDeviceRepository Devices { get; }
        public IMeasurementRepository Measurements { get; }

        public SmartHomeUnitOfWork(SmartHomeDbContext dbContext, IDeviceRepository deviceRepository, IMeasurementRepository measurementRepository)
        {
            _dbContext = dbContext;

            Devices = deviceRepository;
            Measurements = measurementRepository;
        }

        public void Save()
        {
            _dbContext.SaveChanges();
        }
    }
}