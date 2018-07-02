using System.Collections.Generic;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.Devices;
using ESE.SmartHome.Core.Measurements;
using MediatR;

namespace ESE.SmartHome.App.Dashboard
{
    public class ViewDashboard
    {
        public class Query : IRequest<QueryResult>
        { }

        public class QueryResult
        {
            public IDictionary<Device, Measurement> Data { get; set; }
        }

        public class QueryHandler : AsyncRequestHandler<Query, QueryResult>
        {
            private readonly ISmartHomeUnitOfWork _unitOfWork;

            public QueryHandler(ISmartHomeUnitOfWork unitOfWork)
            {
                _unitOfWork = unitOfWork;
            }

            protected override async Task<QueryResult> HandleCore(Query request)
            {
                var dict = new Dictionary<Device, Measurement>();

                var result = await _unitOfWork.Devices.GetAllActiveDevices();

                foreach (var device in result)
                {
                    var m = await _unitOfWork.Measurements.GetByDeviceId(device.Id);

                    dict.Add(device, m);
                }
                
                var queryResult = new QueryResult
                {
                    Data = dict
                };

                return queryResult;
            }
        }
    }
}