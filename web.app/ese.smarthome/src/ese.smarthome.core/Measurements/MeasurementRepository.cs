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
    }

    [UsedImplicitly]
    public class MeasurementRepository : IMeasurementRepository
    {
        private readonly SmartHomeDbContext _context;

        public MeasurementRepository(SmartHomeDbContext context)
        {
            _context = context;
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
            _context.Measurements.Add(measurement);
        }

        private IQueryable<Measurement> Measurements => _context.Measurements;
    }
}