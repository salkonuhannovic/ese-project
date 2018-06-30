using System;
using System.ComponentModel.DataAnnotations;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.Devices;
using ESE.SmartHome.Core.DeviceSettings;
using MediatR;

namespace ESE.SmartHome.App.Setup
{
    public class AddDevice
    {
        public class Command : IRequest<long>
        {
            [Required]
            [StringLength(20, MinimumLength = 3, 
                ErrorMessage = "Device name has to be between 3 and 20 characters long")]
            public string Name { get; set; }

            [Required]
            [RegularExpression("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$", 
                ErrorMessage = "Please insert a valid physical address. Format: xx:xx:xx:xx:xx:xx")]
            public string MacAddress { get; set; }
        }

        public class CommandHandler : AsyncRequestHandler<Command, long>
        {
            private readonly ISmartHomeUnitOfWork _unitOfWork;

            public CommandHandler(ISmartHomeUnitOfWork unitOfWork)
            {
                _unitOfWork = unitOfWork;
            }

            protected override async Task<long> HandleCore(Command command)
            {
                //todo test this
                //NetworkInterface[] arr = NetworkInterface.GetAllNetworkInterfaces();

                //foreach (NetworkInterface item in arr)
                //{
                //    PhysicalAddress mac = item.GetPhysicalAddress();
                //    string stringFormatMac = string.Join(":", mac.GetAddressBytes().Select(varByte => varByte.ToString("X2")));
                //}

                var newDevice = new Device
                {
                    Name = command.Name,
                    Created = DateTime.UtcNow,
                    Setting = new DeviceSetting
                    {
                        MacAddress = command.MacAddress
                    }
                };

                _unitOfWork.Devices.Add(newDevice);
                await _unitOfWork.SaveAsync();

                return newDevice.Id;
            }
        }
    }
}