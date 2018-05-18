using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.Devices;
using MediatR;

namespace ESE.SmartHome.App.Dashboard
{
    public class ViewDashboard
    {
        public class Query : IRequest<QueryResult>
        {
            public TimeSpan TimeSpan { get; set; }
        }

        public class QueryResult
        {
            public Device Device { get; set; }
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
                var dbResult = _unitOfWork.Devices.GetByIdAsync(1).Result;

                var queryResult = new QueryResult
                {
                    Device = dbResult
                };

                return await Task.FromResult(queryResult);
            }
        }
    }
}