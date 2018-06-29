using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Db;
using Microsoft.EntityFrameworkCore;

namespace ESE.SmartHome.Core.Devices
{
    public interface IDeviceRepository
    {
        Task<Device> GetByIdAsync(long id);
        Task<IReadOnlyCollection<Device>> GetAllActiveDevices();
    }

    public class DeviceRepository : IDeviceRepository
    {
        private readonly SmartHomeDbContext _context;

        public DeviceRepository(SmartHomeDbContext context)
        {
            _context = context;
        }

        public async Task<Device> GetByIdAsync(long id)
        {
            return await Devices.FirstOrDefaultAsync(d => d.Id == id);
        }

        public async Task<IReadOnlyCollection<Device>> GetAllActiveDevices()
        {
            return await Devices.Where(d => d.IsActive).ToListAsync();
        }

        private IQueryable<Device> Devices => _context.Devices.Include(s => s.Setting).Include(m => m.Measurements);
    }
}