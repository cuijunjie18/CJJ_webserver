#include <string.h>
#include <iostream>
#include <mysql/mysql.h>

int main(int argc,char *argv[]) {
    MYSQL mysql; //数据库句柄
    MYSQL_RES* res; //查询结果集 
    MYSQL_ROW row; //记录结构体

    //初始化数据库 
    mysql_init(&mysql);
    
    //设置字符编码
    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");
    
    //连接数据库
    if (mysql_real_connect(&mysql, "127.0.0.1", "webdev",
        //"password", "database_name"分别填写自己的的数据库登录密码和表所在的数据库名称
        //这里替换成自己的的数据库等路密码和自己所建的数据库名称即可
        "12345678", "user_info_db", 3306, NULL, 0) == NULL) {
        
        printf("错误原因： %s\n", mysql_error(&mysql));
        printf("连接失败！\n");
        exit(-1);
    }
    
    //查询数据
    int ret = mysql_query(&mysql, "select * from user;");
    printf("ret: %d\n", ret);
    
    //获取结果集
    res = mysql_store_result(&mysql);
    
    //给 ROW 赋值，判断 ROW 是否为空，不为空就打印数据。
    while (row = mysql_fetch_row(res)) {
        printf("%s ", row[0]);
        printf("%s ", row[1]);
        printf("%s ", row[2]);
    }
    printf("\n");
    
    //释放结果集 
    mysql_free_result(res); 
    
    //关闭数据库
    mysql_close(&mysql);
    return 0;
}