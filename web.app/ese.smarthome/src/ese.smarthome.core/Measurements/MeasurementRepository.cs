using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Db;
using JetBrains.Annotations;
using Microsoft.EntityFrameworkCore;

namespace ESE.SmartHome.Core.Measurements
{
    public interface IMeasurementRepository
    {
        Task<ICollection<Measurement>> GetAllForTodayAsync();
        Task<Measurement> GetByDeviceId(long deviceId);

        void Add(Measurement measurement);
        Task UpdateAsync(Measurement measurement);
    }

    [UsedImplicitly]
    public class MeasurementRepository : IMeasurementRepository
    {
        private readonly SmartHomeDbContext _dbContext;

        public MeasurementRepository(SmartHomeDbContext dbContext)
        {
            _dbContext = dbContext;
        }

        public async Task<ICollection<Measurement>> GetAllForTodayAsync()
        {
            return await Measurements.Where(m => m.Timestamp.Date == DateTime.Today).ToListAsync();
        }

        public async Task<Measurement> GetByDeviceId(long deviceId)
        {
            return await Measurements.Where(m => m.DeviceId == deviceId)
                                     .OrderByDescending(m => m.Timestamp)
                                     .FirstOrDefaultAsync();
        }

        public void Add(Measurement measurement)
        {
            _dbContext.Measurements.Add(measurement);
        }

        public async Task UpdateAsync(Measurement measurement)
        {
            _dbContext.Measurements.Update(measurement);
            await _dbContext.SaveChangesAsync();
        }

        private IQueryable<Measurement> Measurements => _dbContext.Measurements;
    }
}