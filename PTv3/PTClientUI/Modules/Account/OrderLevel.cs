using PTEntity;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PortfolioTrading.Modules.Account
{
    public class OrderLevel
    {
        private int _id;

        public int Id
        {
            get { return _id; }
            set { _id = value; }
        }

        private decimal _price;

        public decimal Price
        {
            get { return _price; }
            set { _price = value; }
        }

        private PTEntity.PosiDirectionType _direction;

        public PTEntity.PosiDirectionType Direction
        {
            get { return _direction; }
            set { _direction = value; }
        }

        private string _status;

        public string Status
        {
            get { return _status; }
            set { _status = value; }
        }

        public string GetDisplayText()
        {
            return string.Format("{0}:{1}({2})", Id, Price, Status);
        }

        public static OrderLevel BuildFromJSON(string jsonStr)
        {
            OrderLevel ol = new OrderLevel();

            int startPos = jsonStr.IndexOf('{');
            int endPos = jsonStr.LastIndexOf('}');
            string content = jsonStr.Substring(startPos + 1, endPos - startPos - 1);
            string[] props = content.Split(',');
            foreach (var p in props)
            {
                string[] pair = p.Split(':');
                if (pair.Length > 1)
                {
                    string propName = pair[0].Trim();
                    string propValue = pair[1].Trim();
                    switch (propName)
                    {
                        case "id":
                            ol.Id = int.Parse(propValue);
                            break;
                        case "px":
                            ol.Price = decimal.Parse(propValue);
                            break;
                        case "l/s":
                            ol.Direction =
                                (PTEntity.PosiDirectionType) Enum.Parse(typeof (PTEntity.PosiDirectionType), propValue);
                            break;
                        case "status":
                            ol.Status = DisplayLegStatus((PTEntity.LegStatus) int.Parse(propValue));
                            break;
                    }
                }
            }
            return ol;
        }

        private static string DisplayLegStatus(PTEntity.LegStatus status)
        {
            string ret = "";
            switch (status)
            {
                case LegStatus.UNOPENED:
                    ret = "空仓";
                    break;
                case LegStatus.IS_OPENING:
                    ret = "正开仓";
                    break;
                case LegStatus.OPENED:
                    ret = "已开仓";
                    break;
                case LegStatus.IS_CLOSING:
                    ret = "正平仓";
                    break;
                case LegStatus.CLOSED:
                    ret = "已平仓";
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(status), status, null);
            }
            return ret;
        }
    }
}
