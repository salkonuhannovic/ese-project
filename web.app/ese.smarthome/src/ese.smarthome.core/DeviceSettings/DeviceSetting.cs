using System;
using ESE.SmartHome.Core.Entities;

namespace ESE.SmartHome.Core.DeviceSettings
{
    public class DeviceSetting : Entity
    {
        public string Host { get; set; }
        public string Port { get; set; }
        public string MacAddress { get; set; }
        public int Interval { get; set; }
        public DateTime Created { get; set; }
    }
}