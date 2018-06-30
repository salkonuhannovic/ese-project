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
        Task<IReadOnlyCollection<Device>> GetAllDevices();
        Task<IReadOnlyCollection<Device>> GetAllActiveDevices();

        void Add(Device device);
        Task UpdateAsync(Device device);
        void Delete(Device device);
    }

    public class DeviceRepository : IDeviceRepository
    {
        private readonly SmartHomeDbContext _dbContext;

        public DeviceRepository(SmartHomeDbContext dbContext)
        {
            _dbContext = dbContext;
        }

        public async Task<Device> GetByIdAsync(long id)
        {
            return await Devices.FirstOrDefaultAsync(d => d.Id == id);
        }

        public async Task<IReadOnlyCollection<Device>> GetAllActiveDevices()
        {
            return await Devices.Where(d => d.IsActive).ToListAsync();
        }

        public async Task<IReadOnlyCollection<Device>> GetAllDevices()
        {
            return await Devices.ToListAsync();
        }

        public void Add(Device device)
        {
            _dbContext.Devices.Add(device);
        }

        public async Task UpdateAsync(Device device)
        {
            _dbContext.Devices.Update(device);
            await _dbContext.SaveChangesAsync();
        }

        public void Delete(Device device)
        {
            _dbContext.Devices.Remove(device);
        }

        private IQueryable<Device> Devices => _dbContext.Devices.Include(s => s.Setting);
    }
}