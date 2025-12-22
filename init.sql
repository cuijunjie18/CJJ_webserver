-- 初始化数据库脚本
USE user_info_db;

-- 创建用户表
CREATE TABLE IF NOT EXISTS user(
    username char(50) NULL,
    passwd char(50) NULL
) ENGINE=InnoDB;

-- 插入测试数据
INSERT INTO user(username, passwd) VALUES('manager','1234');

-- 确保权限正确设置
GRANT ALL PRIVILEGES ON user_info_db.* TO 'webdev'@'%';
FLUSH PRIVILEGES;
