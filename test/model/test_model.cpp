#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include "tbl_condition_order.h"

static mysql_api mysql;
static std::string ip = "170.0.45.2";
static std::string usr = "smartinfo";
static std::string pwd = "smartinfo@dfitc2020";
static std::string db = "smartinfo_trade";

void test_insert(tbl_condition_order &tco, int);
void test_update_by_primary(tbl_condition_order &tco);
void test_delete_by_primary(tbl_condition_order &tco);
void test_select_by_primary(tbl_condition_order &tco);
void test_select_by_conditionvalidity(tbl_condition_order &tco);
void test_select(tbl_condition_order &tco);
void test_delete(tbl_condition_order &tco);

int main()
{
    tbl_condition_order tco;
    if (0 != mysql.create(ip, usr, pwd, db, 3306)) {
        return -1;
    }
    tco.set_mysql(&mysql);
    //for (int i = 0;i<10;i++)
    //    test_insert(tco, i+1);
    //test_update_by_primary(tco);
    //test_delete_by_primary(tco);
    test_select_by_primary(tco);
    //test_select_by_conditionvalidity(tco);
    //test_select(tco);
    //test_delete(tco);
    mysql.destroy();
    return 0;
}

void test_delete(tbl_condition_order &tco)
{
    if (tco.del("where conditionorderid = '1'") != 0) {
        std::cout << "del failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
    }
}

void test_select(tbl_condition_order &tco)
{
    if (tco.select("where conditionorderid = '1'") != 0) {
        std::cout << "select_by_conditionvalidity failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
    }
    while (1) {
        auto info = tco.fetch();
        if (info == nullptr) {
            if (tco.code() != 0) {
                std::cout << "select_by_conditionvalidity failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
                return ;
            }
            break;
        }
        std::cout << "requestid = [" << info->requestid << "]" << std::endl;
        std::cout << "conditionorderid = [" << info->conditionorderid.buf << "]" << std::endl;
        std::cout << "triggertype = [" << info->triggertype << "]" << std::endl;
        std::cout << "conditionvalidity = [" << info->conditionvalidity << "]" << std::endl;
        std::cout << "exchangeid = [" << info->exchangeid << "]" << std::endl;
        std::cout << "clientid = [" << info->clientid.buf << "]" << std::endl;
        std::cout << "investorid = [" << info->investorid.buf << "]" << std::endl;
        std::cout << "runningstatus = [" << info->runningstatus << "]" << std::endl;
        std::cout << "triggerstatus = [" << info->triggerstatus << "]" << std::endl;
        std::cout << "contractid = [" << info->contractid.buf << "]" << std::endl;
        std::cout << "direction = [" << info->direction << "]" << std::endl;
        std::cout << "offsetflag = [" << info->offsetflag << "]" << std::endl;
        std::cout << "hedgeflag = [" << info->hedgeflag << "]" << std::endl;
        std::cout << "ordervolume = [" << info->ordervolume << "]" << std::endl;
        std::cout << "pricetype = [" << info->pricetype << "]" << std::endl;
        std::cout << "insertprice = [" << info->insertprice << "]" << std::endl;
        std::cout << "orderref = [" << info->orderref << "]" << std::endl;
        std::cout << "triggerpricetype = [" << info->triggerpricetype << "]" << std::endl;
        std::cout << "triggercomparetype = [" << info->triggercomparetype << "]" << std::endl;
        std::cout << "compareprice = [" << info->compareprice << "]" << std::endl;
        std::cout << "timecondition = [" << info->timecondition.buf << "]" << std::endl;
        std::cout << "statuscondition = [" << info->statuscondition << "]" << std::endl;
        std::cout << "extralconditionexist = [" << info->extralconditionexist << "]" << std::endl;
        std::cout << "extraltriggerpricetype = [" << info->extraltriggerpricetype << "]" << std::endl;
        std::cout << "extraltriggercomparetype = [" << info->extraltriggercomparetype << "]" << std::endl;
        std::cout << "extralcompareprice = [" << info->extralcompareprice << "]" << std::endl;
        std::cout << "maxhand = [" << info->maxhand << "]" << std::endl;
        std::cout << "createtime = [" << info->createtime.buf << "]" << std::endl;
        std::cout << "updatetime = [" << info->updatetime.buf << "]" << std::endl;
        std::cout << "triggertime = [" << info->triggertime.buf << "]" << std::endl;
        std::cout << "relatedorderid = [" << info->relatedorderid.buf << "]" << std::endl;
        std::cout << "retracementdiff = [" << info->retracementdiff << "]" << std::endl;
        std::cout << "pricetick = [" << info->pricetick << "]" << std::endl;
        std::cout << "closefirst = [" << info->closefirst << "]" << std::endl;
        std::cout << "openautostop = [" << info->openautostop << "]" << std::endl;
        std::cout << "splitorder = [" << info->splitorder << "]" << std::endl;
        std::cout << "pluspoint = [" << info->pluspoint << "]" << std::endl;
        std::cout << "priceplusstandard = [" << info->priceplusstandard << "]" << std::endl;
        std::cout << "autostoptype = [" << info->autostoptype << "]" << std::endl;
        std::cout << "stoplosspricetype = [" << info->stoplosspricetype << "]" << std::endl;
        std::cout << "stopprofitpricetype = [" << info->stopprofitpricetype << "]" << std::endl;
        std::cout << "stoplosspoint = [" << info->stoplosspoint << "]" << std::endl;
        std::cout << "stopprofitpoint = [" << info->stopprofitpoint << "]" << std::endl;
        std::cout << "autopriceplus = [" << info->autopriceplus << "]" << std::endl;
        std::cout << "orderresult = [" << info->orderresult << "]" << std::endl;
        std::cout << "orderresultmsg = [" << info->orderresultmsg.buf << "]" << std::endl;
        std::cout << "triggerordervolume = [" << info->triggerordervolume << "]" << std::endl;
        std::cout << "ordervolumetimes = [" << info->ordervolumetimes << "]" << std::endl;
        std::cout << "ordervolumesuccess = [" << info->ordervolumesuccess << "]" << std::endl;
        std::cout << "ordervolumefailed = [" << info->ordervolumefailed << "]" << std::endl;
    }
}

void test_select_by_conditionvalidity(tbl_condition_order &tco)
{
    if (tco.select_by_conditionvalidity(20) != 0) {
        std::cout << "select_by_conditionvalidity failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
    }
    while (1) {
        auto info = tco.fetch_by_conditionvalidity();
        if (info == nullptr) {
            if (tco.code() != 0) {
                std::cout << "select_by_conditionvalidity failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
                return ;
            }
            break;
        }
        std::cout << "requestid = [" << info->requestid << "]" << std::endl;
        std::cout << "conditionorderid = [" << info->conditionorderid.buf << "]" << std::endl;
        std::cout << "triggertype = [" << info->triggertype << "]" << std::endl;
        std::cout << "conditionvalidity = [" << info->conditionvalidity << "]" << std::endl;
        std::cout << "exchangeid = [" << info->exchangeid << "]" << std::endl;
        std::cout << "clientid = [" << info->clientid.buf << "]" << std::endl;
        std::cout << "investorid = [" << info->investorid.buf << "]" << std::endl;
        std::cout << "runningstatus = [" << info->runningstatus << "]" << std::endl;
        std::cout << "triggerstatus = [" << info->triggerstatus << "]" << std::endl;
        std::cout << "contractid = [" << info->contractid.buf << "]" << std::endl;
        std::cout << "direction = [" << info->direction << "]" << std::endl;
        std::cout << "offsetflag = [" << info->offsetflag << "]" << std::endl;
        std::cout << "hedgeflag = [" << info->hedgeflag << "]" << std::endl;
        std::cout << "ordervolume = [" << info->ordervolume << "]" << std::endl;
        std::cout << "pricetype = [" << info->pricetype << "]" << std::endl;
        std::cout << "insertprice = [" << info->insertprice << "]" << std::endl;
        std::cout << "orderref = [" << info->orderref << "]" << std::endl;
        std::cout << "triggerpricetype = [" << info->triggerpricetype << "]" << std::endl;
        std::cout << "triggercomparetype = [" << info->triggercomparetype << "]" << std::endl;
        std::cout << "compareprice = [" << info->compareprice << "]" << std::endl;
        std::cout << "timecondition = [" << info->timecondition.buf << "]" << std::endl;
        std::cout << "statuscondition = [" << info->statuscondition << "]" << std::endl;
        std::cout << "extralconditionexist = [" << info->extralconditionexist << "]" << std::endl;
        std::cout << "extraltriggerpricetype = [" << info->extraltriggerpricetype << "]" << std::endl;
        std::cout << "extraltriggercomparetype = [" << info->extraltriggercomparetype << "]" << std::endl;
        std::cout << "extralcompareprice = [" << info->extralcompareprice << "]" << std::endl;
        std::cout << "maxhand = [" << info->maxhand << "]" << std::endl;
        std::cout << "createtime = [" << info->createtime.buf << "]" << std::endl;
        std::cout << "updatetime = [" << info->updatetime.buf << "]" << std::endl;
        std::cout << "triggertime = [" << info->triggertime.buf << "]" << std::endl;
        std::cout << "relatedorderid = [" << info->relatedorderid.buf << "]" << std::endl;
        std::cout << "retracementdiff = [" << info->retracementdiff << "]" << std::endl;
        std::cout << "pricetick = [" << info->pricetick << "]" << std::endl;
        std::cout << "closefirst = [" << info->closefirst << "]" << std::endl;
        std::cout << "openautostop = [" << info->openautostop << "]" << std::endl;
        std::cout << "splitorder = [" << info->splitorder << "]" << std::endl;
        std::cout << "pluspoint = [" << info->pluspoint << "]" << std::endl;
        std::cout << "priceplusstandard = [" << info->priceplusstandard << "]" << std::endl;
        std::cout << "autostoptype = [" << info->autostoptype << "]" << std::endl;
        std::cout << "stoplosspricetype = [" << info->stoplosspricetype << "]" << std::endl;
        std::cout << "stopprofitpricetype = [" << info->stopprofitpricetype << "]" << std::endl;
        std::cout << "stoplosspoint = [" << info->stoplosspoint << "]" << std::endl;
        std::cout << "stopprofitpoint = [" << info->stopprofitpoint << "]" << std::endl;
        std::cout << "autopriceplus = [" << info->autopriceplus << "]" << std::endl;
        std::cout << "orderresult = [" << info->orderresult << "]" << std::endl;
        std::cout << "orderresultmsg = [" << info->orderresultmsg.buf << "]" << std::endl;
        std::cout << "triggerordervolume = [" << info->triggerordervolume << "]" << std::endl;
        std::cout << "ordervolumetimes = [" << info->ordervolumetimes << "]" << std::endl;
        std::cout << "ordervolumesuccess = [" << info->ordervolumesuccess << "]" << std::endl;
        std::cout << "ordervolumefailed = [" << info->ordervolumefailed << "]" << std::endl;
    }
}

void test_select_by_primary(tbl_condition_order &tco)
{
    if (tco.select_by_primary((char*)"1") != 0) {
        std::cout << "update_by_primary failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
    }
    while (1) {
        auto info = tco.fetch_by_primary();
        if (info == nullptr) {
            if (tco.code() != 0) {
                std::cout << "update_by_primary failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
                return ;
            }
            break;
        }
        std::cout << "requestid = [" << info->requestid << "]" << std::endl;
        std::cout << "conditionorderid = [" << info->conditionorderid.buf << "]" << std::endl;
        std::cout << "triggertype = [" << info->triggertype << "]" << std::endl;
        std::cout << "conditionvalidity = [" << info->conditionvalidity << "]" << std::endl;
        std::cout << "exchangeid = [" << info->exchangeid << "]" << std::endl;
        std::cout << "clientid = [" << info->clientid.buf << "]" << std::endl;
        std::cout << "investorid = [" << info->investorid.buf << "]" << std::endl;
        std::cout << "runningstatus = [" << info->runningstatus << "]" << std::endl;
        std::cout << "triggerstatus = [" << info->triggerstatus << "]" << std::endl;
        std::cout << "contractid = [" << info->contractid.buf << "]" << std::endl;
        std::cout << "direction = [" << info->direction << "]" << std::endl;
        std::cout << "offsetflag = [" << info->offsetflag << "]" << std::endl;
        std::cout << "hedgeflag = [" << info->hedgeflag << "]" << std::endl;
        std::cout << "ordervolume = [" << info->ordervolume << "]" << std::endl;
        std::cout << "pricetype = [" << info->pricetype << "]" << std::endl;
        std::cout << "insertprice = [" << info->insertprice << "]" << std::endl;
        std::cout << "orderref = [" << info->orderref << "]" << std::endl;
        std::cout << "triggerpricetype = [" << info->triggerpricetype << "]" << std::endl;
        std::cout << "triggercomparetype = [" << info->triggercomparetype << "]" << std::endl;
        std::cout << "compareprice = [" << info->compareprice << "]" << std::endl;
        std::cout << "timecondition = [" << info->timecondition.buf << "]" << std::endl;
        std::cout << "statuscondition = [" << info->statuscondition << "]" << std::endl;
        std::cout << "extralconditionexist = [" << info->extralconditionexist << "]" << std::endl;
        std::cout << "extraltriggerpricetype = [" << info->extraltriggerpricetype << "]" << std::endl;
        std::cout << "extraltriggercomparetype = [" << info->extraltriggercomparetype << "]" << std::endl;
        std::cout << "extralcompareprice = [" << info->extralcompareprice << "]" << std::endl;
        std::cout << "maxhand = [" << info->maxhand << "]" << std::endl;
        std::cout << "createtime = [" << info->createtime.buf << "]" << std::endl;
        std::cout << "updatetime = [" << info->updatetime.buf << "]" << std::endl;
        std::cout << "triggertime = [" << info->triggertime.buf << "]" << std::endl;
        std::cout << "relatedorderid = [" << info->relatedorderid.buf << "]" << std::endl;
        std::cout << "retracementdiff = [" << info->retracementdiff << "]" << std::endl;
        std::cout << "pricetick = [" << info->pricetick << "]" << std::endl;
        std::cout << "closefirst = [" << info->closefirst << "]" << std::endl;
        std::cout << "openautostop = [" << info->openautostop << "]" << std::endl;
        std::cout << "splitorder = [" << info->splitorder << "]" << std::endl;
        std::cout << "pluspoint = [" << info->pluspoint << "]" << std::endl;
        std::cout << "priceplusstandard = [" << info->priceplusstandard << "]" << std::endl;
        std::cout << "autostoptype = [" << info->autostoptype << "]" << std::endl;
        std::cout << "stoplosspricetype = [" << info->stoplosspricetype << "]" << std::endl;
        std::cout << "stopprofitpricetype = [" << info->stopprofitpricetype << "]" << std::endl;
        std::cout << "stoplosspoint = [" << info->stoplosspoint << "]" << std::endl;
        std::cout << "stopprofitpoint = [" << info->stopprofitpoint << "]" << std::endl;
        std::cout << "autopriceplus = [" << info->autopriceplus << "]" << std::endl;
        std::cout << "orderresult = [" << info->orderresult << "]" << std::endl;
        std::cout << "orderresultmsg = [" << info->orderresultmsg.buf << "]" << std::endl;
        std::cout << "triggerordervolume = [" << info->triggerordervolume << "]" << std::endl;
        std::cout << "ordervolumetimes = [" << info->ordervolumetimes << "]" << std::endl;
        std::cout << "ordervolumesuccess = [" << info->ordervolumesuccess << "]" << std::endl;
        std::cout << "ordervolumefailed = [" << info->ordervolumefailed << "]" << std::endl;
    }
}

void test_delete_by_primary(tbl_condition_order &tco)
{
    tbl_condition_order_info *info = tco.get_info();
    if (tco.del_by_primary((char*)"1") != 0) {
        std::cout << "update_by_primary failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
    }
}

void test_update_by_primary(tbl_condition_order &tco)
{
    tbl_condition_order_info *info = tco.get_info();
    info->requestid = 1;
    strcpy(info->conditionorderid.buf, "1");
    info->conditionorderid.len = strlen(info->conditionorderid.buf);
    info->triggertype = 1;
    info->conditionvalidity = 1;
    info->exchangeid = 1;
    strcpy(info->clientid.buf, "1");
    info->clientid.len = strlen(info->clientid.buf);
    strcpy(info->investorid.buf, "1");
    info->investorid.len = strlen(info->investorid.buf);
    info->runningstatus = 1;
    info->triggerstatus = 1;
    strcpy(info->contractid.buf, "1");
    info->contractid.len = strlen(info->contractid.buf);
    info->direction = 1;
    info->offsetflag = 1;
    info->hedgeflag = 1;
    info->ordervolume = 1;
    info->pricetype = 1;
    info->insertprice = 1.0;
    info->orderref = 1;
    info->triggerpricetype = 1;
    info->triggercomparetype = 1;
    info->compareprice = 1.0;
    strcpy(info->timecondition.buf, "1");
    info->timecondition.len = strlen(info->timecondition.buf);
    info->statuscondition = 1;;
    info->extralconditionexist = 1;
    info->extraltriggerpricetype = 1;
    info->extraltriggercomparetype = 1;
    info->extralcompareprice = 1.0;
    info->maxhand = 1;
    strcpy(info->createtime.buf, "2020-02-05_16:30:00.123456");
    info->createtime.len = strlen(info->createtime.buf);
    strcpy(info->updatetime.buf, "2020-02-05_18:30:00.123456");
    info->updatetime.len = strlen(info->updatetime.buf);
    strcpy(info->triggertime.buf, "12:00");
    info->triggertime.len = strlen(info->triggertime.buf);
    strcpy(info->relatedorderid.buf, "1");
    info->relatedorderid.len = strlen(info->relatedorderid.buf);
    info->retracementdiff = 1.0;
    info->pricetick = 1.0;
    info->closefirst = 1;
    info->openautostop = 1;
    info->splitorder = 1;
    info->pluspoint = 1;
    info->priceplusstandard = 1;
    info->autostoptype = 1;
    info->stoplosspricetype = 1;
    info->stopprofitpricetype = 1;
    info->stoplosspoint = 1;
    info->stopprofitpoint = 1;
    info->autopriceplus = 1.0;
    info->orderresult = 1;
    strcpy(info->orderresultmsg.buf, "1");
    info->orderresultmsg.len = strlen(info->orderresultmsg.buf);
    info->triggerordervolume = 1;
    info->ordervolumetimes = 1;
    info->ordervolumesuccess = 1;
    info->ordervolumefailed = 1;
    if (tco.update_by_primary((char*)"1") != 0) {
        std::cout << "update_by_primary failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
    }
}

void test_insert(tbl_condition_order &tco, int i)
{
    char idx[32];
    sprintf(idx, "%d", i);
    tbl_condition_order_info *info = tco.get_info();
    info->requestid = i;
    strcpy(info->conditionorderid.buf, idx);
    info->conditionorderid.len = strlen(info->conditionorderid.buf);
    info->triggertype = i;
    info->conditionvalidity = i;
    info->exchangeid = i;
    strcpy(info->clientid.buf, idx);
    info->clientid.len = strlen(info->clientid.buf);
    strcpy(info->investorid.buf, idx);
    info->investorid.len = strlen(info->investorid.buf);
    info->runningstatus = i;
    info->triggerstatus = i;
    strcpy(info->contractid.buf, idx);
    info->contractid.len = strlen(info->contractid.buf);
    info->direction = i;
    info->offsetflag = i;
    info->hedgeflag = i;
    info->ordervolume = i;
    info->pricetype = i;
    info->insertprice = (double)i;
    info->orderref = i;
    info->triggerpricetype = i;
    info->triggercomparetype = i;
    info->compareprice = (double)i;
    strcpy(info->timecondition.buf, idx);
    info->timecondition.len = strlen(info->timecondition.buf);
    info->statuscondition = i;;
    info->extralconditionexist = i;
    info->extraltriggerpricetype = i;
    info->extraltriggercomparetype = i;
    info->extralcompareprice = (double)i;
    info->maxhand = i;
    if (i%2) {
        strcpy(info->createtime.buf, "2020-02-05_16:30:00.123456");
    }
    else {
        strcpy(info->createtime.buf, "2019-02-14_16:30:00.123456");
    }
    info->createtime.len = strlen(info->createtime.buf);
    strcpy(info->updatetime.buf, "2020-02-05_16:30:00.123456");
    info->updatetime.len = strlen(info->updatetime.buf);
    strcpy(info->triggertime.buf, "12:00");
    info->triggertime.len = strlen(info->triggertime.buf);
    strcpy(info->relatedorderid.buf, idx);
    info->relatedorderid.len = strlen(info->relatedorderid.buf);
    info->retracementdiff = (double)i;
    info->pricetick = (double)i;
    info->closefirst = i;
    info->openautostop = i;
    info->splitorder = i;
    info->pluspoint = i;
    info->priceplusstandard = i;
    info->autostoptype = i;
    info->stoplosspricetype = i;
    info->stopprofitpricetype = i;
    info->stoplosspoint = i;
    info->stopprofitpoint = i;
    info->autopriceplus = (double)i;
    info->orderresult = i;
    strcpy(info->orderresultmsg.buf, idx);
    info->orderresultmsg.len = strlen(info->orderresultmsg.buf);
    info->triggerordervolume = i;
    info->ordervolumetimes = i;
    info->ordervolumesuccess = i;
    info->ordervolumefailed = i;
    if (tco.insert() != 0) {
        std::cout << "insert failed, code = " << tco.code() << ", msg = " << tco.msg() << std::endl;
    }
}