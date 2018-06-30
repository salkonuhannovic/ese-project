using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.DeviceSettings;
using MediatR;
using Microsoft.AspNetCore.Mvc.Rendering;
using Microsoft.Extensions.Logging;

namespace ESE.SmartHome.App.Setup
{
    public class EditDevice
    {
        [SuppressMessage("ReSharper", "UnusedAutoPropertyAccessor.Global")]
        public class Query : IRequest<Command>
        {
            public long DeviceId { get; set; }
        }

        public class Command : IRequest
        {
            public long DeviceId { get; set; }
            public string Name { get; set; }
            public bool IsActive { get; set; }
            public DeviceSetting Setting { get; set; }
        }

        public class QueryHandler : AsyncRequestHandler<Query, Command>
        {
            private readonly ISmartHomeUnitOfWork _unitOfWork;

            public QueryHandler(ISmartHomeUnitOfWork unitOfWork)
            {
                _unitOfWork = unitOfWork;
            }

            protected override async Task<Command> HandleCore(Query request)
            {
                var device = await _unitOfWork.Devices.GetByIdAsync(request.DeviceId);

                if (device == null)
                {
                    throw new Exception();
                }

                var result = new Command
                {
                    DeviceId = device.Id,
                    Name = device.Name,
                    IsActive = device.IsActive,
                    Setting = new DeviceSetting
                    {
                        MacAddress = device.Setting.MacAddress
                    }
                };

                return result;
            }
        }

        public class CommandHandler : AsyncRequestHandler<Command>
        {
            private readonly ISmartHomeUnitOfWork _unitOfWork;
            private readonly ILogger<CommandHandler> _logger;

            public CommandHandler(ISmartHomeUnitOfWork unitOfWork, ILogger<CommandHandler> logger)
            {
                _unitOfWork = unitOfWork;
                _logger = logger;
            }

            protected override async Task HandleCore(Command request)
            {
                var device = await _unitOfWork.Devices.GetByIdAsync(request.DeviceId);

                // TODO: Should be exception (can't do anything in this case)
                if (device == null)
                {
                    _logger.LogWarning("Device with id [{DeviceId}] to update not found.", request.DeviceId);
                    throw new Exception();
                }

                device.Name = request.Name;
                device.IsActive = request.IsActive;
                device.Setting = new DeviceSetting
                {
                    MacAddress = request.Setting.MacAddress
                };

                await _unitOfWork.Devices.UpdateAsync(device);
            }
        }
    }
}