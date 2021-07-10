#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <functional>
#include <cfloat>
#include <math.h>

#include <memory>
#include <chrono>

using namespace std;


enum OrderAction
{
    ACTION_UNDEFINED,
    NEW,
    MODIFY,
    CANCEL,
    PRINT
};

enum OrderSide
{
    SIDE_UNDEFINED,
    BUY,
    SELL,
};

enum OrderType
{
    TYPE_UNDEFINED,
    GFD,
    IOC
};

enum OrderState
{
    STATE_UNDEFINED,
    ACK,
    CANCELLED,
    TRADED
};

class Order
{
    public:
    Order(std::string const & order_str_) 
        : _orderStr(order_str_),
        _orderId(""),
        _orderAction(ACTION_UNDEFINED),
        _orderSide(SIDE_UNDEFINED),
        _orderType(TYPE_UNDEFINED),
        _orderState(STATE_UNDEFINED),
        _orderPrice(0), //taking zero as unitiliazed price as its not an allowed price
        _orderQty(0),//taking zero as unitiliazed qty as its not an allowed qty
        _orderValid(false)
    {
        _orderTimeStamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        _orderValid = BuildOrderFromStr();
    }
    Order(std::string const & order_str_,
          std::string const & order_id_,
          OrderAction act_,
          OrderSide side_,
          OrderType type_,
          OrderState state_,
          int prc_,
          int qty_)
        : _orderStr(order_str_),
        _orderId(order_id_),
        _orderAction(act_),
        _orderSide(side_),
        _orderType(type_),
        _orderState(state_),
        _orderPrice(prc_), 
        _orderQty(qty_),
        _orderValid(true)
    {
        _orderTimeStamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }


    std::string getOrderStr() { return _orderStr;}
    std::string getId() { return _orderId;}
    OrderAction getOrderAction() { return _orderAction;}
    OrderSide getOrderSide() { return _orderSide;}
    OrderType getOrderType() { return _orderType;}
    OrderState getOrderState() { return _orderState;}
    int getPrice() { return _orderPrice;}
    int getQty() { return _orderQty;}
    unsigned long long getTimeStamp() { return _orderTimeStamp;}
    bool isValid() { return _orderValid;}

    bool Trade(int trdQty_)
    {
        bool result = false;
        _orderQty = _orderQty - trdQty_;
        if(_orderQty == 0)
        {
            _orderState = TRADED;
            result = true;
        }
        return result;
    }

    bool SetOrderAction(std::string const & act_str_)
    {
        bool result = true;
        if(act_str_ == "BUY")
        {
            _orderAction = NEW;
            _orderSide = BUY;
        }
        else if(act_str_ == "SELL")
        {
            _orderAction = NEW;
            _orderSide = SELL;
        }   
        else if(act_str_ == "MODIFY")
        {
            _orderAction = MODIFY;
        }   
        else if(act_str_ == "CANCEL")
        {
            _orderAction = CANCEL;
        }   
        else if(act_str_ == "PRINT")
        {
            _orderAction = PRINT;
        }   
        else
        {
            _orderAction = ACTION_UNDEFINED;
            result = false;
        }
        return result;
    }

    bool SetOrderType(std::string const & type_str_)
    {
        bool result = true;
        if(type_str_ == "GFD")
        {
            _orderType = GFD;
        }
        else if(type_str_ == "IOC")
        {
            _orderType = IOC;
        }   
        else
        {
            _orderType = TYPE_UNDEFINED;
            result = false;
        }
        return result;
    }

    bool SetOrderNumVals(std::string const & price_str_, int & dest_)
    {
        bool result = true;
        try {
        dest_ = std::stoi(price_str_);
        } catch (const std::invalid_argument&) {
            std::cerr << "Numeric value is invalid : " << price_str_ << std::endl;
            return false;
        } catch (const std::out_of_range&) {
            std::cerr << "Numeric value is out of range for an integer : " << price_str_ << std::endl;
            return false;
        }
        catch(...) {
            // everything else
            return false;
        }
        if(dest_ <= 0)
        {
            return false;
        }
        return result;
    }

    void SetOrderState(OrderState state_)
    {
        _orderState = state_;
    }
            
    bool BuildOrderFromStr()
    {
        std::stringstream ss(_orderStr);

        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> vstrings(begin, end);
        if(vstrings.empty())
        {
            return false;
        }

        if(!SetOrderAction(vstrings[0]))
        {
            return false;
        }
        int ordStrSize = vstrings.size();

        switch(_orderAction)
        {
        case NEW:
            if(ordStrSize != 5)
            {
                return false;
            }
            if(!SetOrderType(vstrings[1]))
            {
                return false;
            }
            if(!SetOrderNumVals(vstrings[2],_orderPrice))
            {
                return false;
            }
            if(!SetOrderNumVals(vstrings[3],_orderQty))
            {
                return false;
            }
            if(!vstrings[4].empty())
            {
                _orderId = vstrings[4];
            }
            else
            {
                return false;
            }

            break;
        case MODIFY:
            if(ordStrSize != 5)
            {
                return false;
            }
            if(!vstrings[1].empty())
            {
                _orderId = vstrings[1];
            }
            else
            {
                return false;
            }
            if(vstrings[2] == "BUY")
            {
                _orderSide = BUY;
            }
            else if(vstrings[2] == "SELL")
            {
                _orderSide = SELL;
            }
            else
            {
                return false;
            }
            if(!SetOrderNumVals(vstrings[3],_orderPrice))
            {
                return false;
            }
            if(!SetOrderNumVals(vstrings[4],_orderQty))
            {
                return false;
            }
            break;
        case CANCEL:
            if(ordStrSize != 2)
            {
                return false;
            }
            if(!vstrings[1].empty())
            {
                _orderId = vstrings[1];
            }
            else
            {
                return false;
            }
            break;
        case PRINT:
            if(ordStrSize != 1) //sanity
            {
                return false;
            }
            break;
        default:    
            return false;
            break;
        }
        return true;
    }

    bool operator < (const Order& rhs_)
    {
        //Price Time ordering - Buy
        bool result = false;
        if(_orderPrice < rhs_._orderPrice)
        {
            return true;
        }
        else if(_orderPrice == rhs_._orderPrice)
        {
            return (_orderTimeStamp > rhs_._orderTimeStamp);
        }
        return result;
    }

    bool operator > (const Order& rhs_)
    {
        //Price Time ordering - Sell
        bool result = false;
        if(_orderPrice > rhs_._orderPrice)
        {
            return true;
        }
        else if(_orderPrice == rhs_._orderPrice)
        {
            return (_orderTimeStamp > rhs_._orderTimeStamp);
        }
        return result;
    }

    void ShowOrder()
    {
        std::cout << "_orderStr=" << _orderStr
            << ";_orderValid=" << _orderValid
            << ";_orderId=" << _orderId
            << ";_orderAction=" << _orderAction
            << ";_orderSide=" << _orderSide
            << ";_orderType=" << _orderType
            << ";_orderState=" << _orderState
            << ";_orderPrice=" << _orderPrice
            << ";_orderQty=" << _orderQty
            << ";_orderTimeStamp=" << _orderTimeStamp
            << std::endl;
    }


    private:
    std::string _orderStr;   
    std::string _orderId;
    OrderAction _orderAction; 
    OrderSide _orderSide; 
    OrderType _orderType;
    OrderState _orderState;
    int _orderPrice;
    int  _orderQty;
    unsigned long long _orderTimeStamp;
    bool _orderValid;
};

class Trade
{
    public:
    Trade(std::pair<std::string,std::string> orderIds_,
                std::pair<int,int> priceVals_,
                std::pair<int,int> qtyVals_)
            : _orderIds(orderIds_),
            _priceVals(priceVals_),
            _qtyVals(qtyVals_)
    {
    }
    ~Trade() {}
    void ShowTrade()
    {
        std::cout << "TRADE " << _orderIds.first
            << " " << _priceVals.first
            << " " << _qtyVals.first
            << " " << _orderIds.second
            << " " << _priceVals.second
            << " " << _qtyVals.second
            << std::endl;
    }
    private:
    std::pair<std::string,std::string> _orderIds;     
    std::pair<int,int> _priceVals;     
    std::pair<int,int> _qtyVals;     
};

namespace OrderTypes
{
    typedef std::shared_ptr<Order> OrdPtr;
    class OrderCompBuy
    {
        public:
        bool operator() (OrdPtr r,OrdPtr l)    
        {
            return (*r) < (*l);
        }
    };
    class OrderCompSell
    {
        public:
        bool operator() (OrdPtr r,OrdPtr l)    
        {
            return (*r) > (*l);
        }
    };
    typedef std::priority_queue<OrderTypes::OrdPtr, std::vector<OrderTypes::OrdPtr>, OrderTypes::OrderCompBuy> BuyQ;
    typedef std::priority_queue<OrderTypes::OrdPtr, std::vector<OrderTypes::OrdPtr>, OrderTypes::OrderCompSell> SellQ;
}

class BookBuilder
{
    public:
        BookBuilder() {}
        ~BookBuilder() {}
        bool Execute(OrderTypes::OrdPtr oPtr)
        {
            bool result = true;
            if(!oPtr->isValid())
            {
                return false;
            }
            std::vector<std::shared_ptr<Trade>> trades;

            switch(oPtr->getOrderAction())
            {
            case NEW:
                //Ignore new Order if Id is allready present
                if(_orderCache.find(oPtr->getId()) == _orderCache.end())
                {
                    oPtr->SetOrderState(ACK);

                    if(oPtr->getOrderType() == GFD)
                    {
                        _orderCache[oPtr->getId()] = oPtr;
                        if(oPtr->getOrderSide() == BUY)
                        {
                            _buyQ.push(oPtr);
                        }
                        else if(oPtr->getOrderSide() == SELL)
                        {
                            _sellQ.push(oPtr);
                        }
                        Match(_buyQ,_sellQ,trades);
                    }
                    else if(oPtr->getOrderType() == IOC)
                    {
                        if(oPtr->getOrderSide() == BUY)
                        {
                            OrderTypes::BuyQ tempBuyQ;
                            tempBuyQ.push(oPtr);
                            Match(tempBuyQ,_sellQ,trades);
                        }
                        else if(oPtr->getOrderSide() == SELL)
                        {
                            OrderTypes::SellQ tempSellQ;
                            tempSellQ.push(oPtr);
                            Match(_buyQ,tempSellQ,trades);
                        }
                    }
                }
                break;
            case MODIFY:
                //set order state of existing one cancelled and generate a new one with same id
                if(_orderCache.find(oPtr->getId()) != _orderCache.end())
                {
                    _orderCache[oPtr->getId()]->SetOrderState(CANCELLED);
                    _orderCache.erase(oPtr->getId());
                    OrderTypes::OrdPtr modOrdPtr(new Order(oPtr->getOrderStr(),
                                                           oPtr->getId(),
                                                           NEW,
                                                           oPtr->getOrderSide(),
                                                           GFD,
                                                           ACK,
                                                           oPtr->getPrice(),
                                                           oPtr->getQty()));
                    this->Execute(modOrdPtr);
                }

                break;
            case CANCEL:
                if(_orderCache.find(oPtr->getId()) != _orderCache.end())
                {
                    OrderTypes::OrdPtr cachePtr = _orderCache[oPtr->getId()];
                    cachePtr->SetOrderState(CANCELLED);
                    _orderCache.erase(oPtr->getId());
                }
                break;
            case PRINT:
                Print();
                break;
            default:    
                return false;
                break;
            }
            for(auto si = trades.begin();si != trades.end();++si)
            {
                (*si)->ShowTrade();
            }
            return result;
        }

        void Print() const
        {
            std::cout << "SELL:" << std::endl;
            auto tSellQ = _sellQ; //copy
            std::vector<std::pair<int,int> > sellAgg;
            while(!tSellQ.empty())
            {
                if(tSellQ.top()->getOrderState() != CANCELLED)
                {
                    int curPrice = tSellQ.top()->getPrice();
                    int curQty = tSellQ.top()->getQty();
                    int sVecSize = sellAgg.size();
                    if((sVecSize > 0))
                    {
                        if(sellAgg[sVecSize - 1].first == curPrice)
                        {
                            sellAgg[sVecSize - 1].second += curQty;
                        }
                        else
                        {
                            sellAgg.push_back(std::make_pair(curPrice,curQty));
                        }
                    }
                    else
                    {
                        sellAgg.push_back(std::make_pair(curPrice,curQty));
                    }
                }
                tSellQ.pop();
            }
            std::reverse(sellAgg.begin(),sellAgg.end());
            for(auto si = sellAgg.begin();si != sellAgg.end();++si)
            {
                std::cout << si->first << " " << si->second << std::endl;
            }

            std::cout << "BUY:" << std::endl;
            auto tBuyQ = _buyQ; //copy
            std::vector<std::pair<int,int> > buyAgg;
            while(!tBuyQ.empty())
            {
                if(tBuyQ.top()->getOrderState() != CANCELLED)
                {
                    int curPrice = tBuyQ.top()->getPrice();
                    int curQty = tBuyQ.top()->getQty();
                    int bVecSize = buyAgg.size();
                    if((bVecSize > 0))
                    {
                        if(buyAgg[bVecSize - 1].first == curPrice)
                        {
                            buyAgg[bVecSize - 1].second += curQty;
                        }
                        else
                        {
                            buyAgg.push_back(std::make_pair(curPrice,curQty));
                        }
                    }
                    else
                    {
                        buyAgg.push_back(std::make_pair(curPrice,curQty));
                    }
                }
                tBuyQ.pop();
            }
            for(auto si = buyAgg.begin();si != buyAgg.end();++si)
            {
                std::cout << si->first << " " << si->second << std::endl;
            }
        }

        void PrintDebug() const
        {
            std::cout << "BUY Q " << std::endl;
            auto tBuyQ = _buyQ; //copy
            while(!tBuyQ.empty())
            {
                tBuyQ.top()->ShowOrder();
                tBuyQ.pop();
            }

            std::cout << "SELL Q " << std::endl;
            auto tSellQ = _sellQ; //copy
            while(!tSellQ.empty())
            {
                tSellQ.top()->ShowOrder();
                tSellQ.pop();
            }
        }

        void FlushQ(OrderTypes::BuyQ& buyQ_, OrderTypes::SellQ& sellQ_)
        {
            //Flush Cancel at top of book
            while(!buyQ_.empty())
            {
                if(buyQ_.top()->getOrderState() == CANCELLED)
                {
                    buyQ_.pop();
                }
                else
                {
                    break;
                }
            }
            while(!sellQ_.empty())
            {
                if(sellQ_.top()->getOrderState() == CANCELLED)
                {
                    sellQ_.pop();
                }
                else
                {
                    break;
                }
            }
        }

        void Match(OrderTypes::BuyQ& buyQ_, OrderTypes::SellQ& sellQ_, std::vector<std::shared_ptr<Trade>> & trades_)
        {
            FlushQ(buyQ_,sellQ_);
            //Loop while price-cross
            if((!buyQ_.empty()) && (!sellQ_.empty()))
            {
                while(buyQ_.top()->getPrice() >= sellQ_.top()->getPrice())
                {
                    //This means trade , so lets do it
                    OrderTypes::OrdPtr bOrd = buyQ_.top();
                    OrderTypes::OrdPtr sOrd = sellQ_.top();
                    OrderTypes::OrdPtr ordPtr1,ordPtr2;
                    if(bOrd->getTimeStamp() < sOrd->getTimeStamp())
                    {
                        ordPtr1 = bOrd;
                        ordPtr2 = sOrd;
                    }
                    else
                    {
                        ordPtr1 = sOrd;
                        ordPtr2 = bOrd;
                    }
                    int trdQty = std::min(ordPtr1->getQty(),ordPtr2->getQty());
                    if(bOrd->Trade(trdQty))
                    {
                        buyQ_.pop();
                        _orderCache.erase(bOrd->getId());
                    }
                    if(sOrd->Trade(trdQty))
                    {
                        sellQ_.pop();
                        _orderCache.erase(sOrd->getId());
                    }
                    std::shared_ptr<Trade> trdPtr(new Trade(std::make_pair(ordPtr1->getId(),ordPtr2->getId()),
                                                            std::make_pair(ordPtr1->getPrice(),ordPtr2->getPrice()),
                                                            std::make_pair(trdQty,trdQty)));
                    trades_.push_back(trdPtr);
                    FlushQ(buyQ_,sellQ_);
                    if(buyQ_.empty() || sellQ_.empty())
                    {
                        break;
                    }

                }
            }
        }
        void Clean()
        {
            _orderCache.clear();
            _buyQ = OrderTypes::BuyQ();
            _sellQ = OrderTypes::SellQ();
        }

    private:
        std::unordered_map<std::string,OrderTypes::OrdPtr > _orderCache;
        OrderTypes::BuyQ _buyQ;
        OrderTypes::SellQ _sellQ;
};



int main() {
    //for (std::string line; std::getline(std::cin, line);) 
    //{
    //  //added something
    //}


    BookBuilder bb;

    /////////////////////////
    std::cout << "Example 1" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 2" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 20 order2"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 3" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1001 20 order2"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 4" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("SELL GFD 900 20 order2"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 5" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1010 10 order2"));
    bb.Execute(std::make_shared<Order>("SELL GFD 1000 15 order3"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 6" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order2"));
    bb.Execute(std::make_shared<Order>("SELL GFD 900 20 order3"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 7" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("MODIFY order1 SELL 1000 10"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 8" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order2"));
    bb.Execute(std::make_shared<Order>("MODIFY order1 BUY 1000 20"));
    bb.Execute(std::make_shared<Order>("SELL GFD 900 20 order3"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 9" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY IOC 1000 10 order2"));
    bb.Execute(std::make_shared<Order>("MODIFY order1 BUY 1000 20"));
    bb.Execute(std::make_shared<Order>("SELL GFD 900 20 order3"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 10" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY IOC 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order2"));
    bb.Execute(std::make_shared<Order>("MODIFY order1 BUY 1000 20"));
    bb.Execute(std::make_shared<Order>("SELL IOC 900 20 order3"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 11" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order2"));
    bb.Execute(std::make_shared<Order>("CANCEL order2"));
    bb.Execute(std::make_shared<Order>("MODIFY order1 BUY 1000 20"));
    bb.Execute(std::make_shared<Order>("SELL IOC 900 20 order3"));
    bb.Execute(std::make_shared<Order>("PRINT"));
    bb.Clean();
    /////////////////////////
    std::cout << "Example 12" << std::endl;
    bb.Execute(std::make_shared<Order>("BUY GFD 1000 10 order1"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1100 20 order2"));
    bb.Execute(std::make_shared<Order>("BUY GFD 900 30 order3"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1100 30 order4"));
    bb.Execute(std::make_shared<Order>("BUY GFD 1100 40 order5"));
    bb.Execute(std::make_shared<Order>("MODIFY order4 BUY 1100 40"));
    //bb.PrintDebug();
    bb.Execute(std::make_shared<Order>("SELL GFD 1100 100 order6"));
    bb.Clean();
    


    return 0;
}

    
