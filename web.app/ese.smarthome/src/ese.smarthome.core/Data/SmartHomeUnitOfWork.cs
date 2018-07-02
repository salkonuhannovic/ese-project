using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Db;
using ESE.SmartHome.Core.Devices;
using ESE.SmartHome.Core.Measurements;
using JetBrains.Annotations;
using Microsoft.EntityFrameworkCore;

namespace ESE.SmartHome.Core.Data
{
    public interface ISmartHomeUnitOfWork
    {
        IDeviceRepository Devices { get; }
        IMeasurementRepository Measurements { get; }

        void Save();
        Task<int> SaveAsync();
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

        public async Task<int> SaveAsync() => await _dbContext.SaveChangesAsync();
    }
}