# Docker 部署指南

本项目已添加Docker支持，可以通过Docker容器化部署CJJ WebServer。

## 文件说明

- `Dockerfile`: 构建WebServer应用镜像的配置文件
- `docker-compose.yml`: Docker Compose配置文件，包含WebServer和MySQL服务
- `init.sql`: MySQL数据库初始化脚本
- `.dockerignore`: Docker构建时忽略的文件列表

## 快速开始

### 方式一：使用 docker-compose（推荐）

1. **启动完整服务栈**（包括MySQL数据库和WebServer）:
   ```bash
   docker-compose up -d
   ```

2. **查看服务状态**:
   ```bash
   docker-compose ps
   ```

3. **访问应用**:
   打开浏览器访问 `http://localhost:50001`

4. **停止服务**:
   ```bash
   docker-compose down
   ```

### 方式二：分步构建和运行

1. **构建WebServer镜像**:
   ```bash
   docker build -t cjj-webserver .
   ```

2. **启动MySQL容器**:
   ```bash
   docker run -d \
     --name mysql-server \
     -e MYSQL_ROOT_PASSWORD=rootpassword \
     -e MYSQL_DATABASE=user_info_db \
     -e MYSQL_USER=webdev \
     -e MYSQL_PASSWORD=12345678 \
     -p 3306:3306 \
     -v $(pwd)/init.sql:/docker-entrypoint-initdb.d/init.sql \
     mysql:8.0
   ```

3. **启动WebServer容器**:
   ```bash
   docker run -d \
     --name cjj-webserver \
     -p 50001:50001 \
     --link mysql-server:mysql \
     -e MYSQL_HOST=mysql \
     -e MYSQL_PORT=3306 \
     cjj-webserver
   ```

## 配置说明

### 环境变量

WebServer支持以下环境变量配置：

- `MYSQL_HOST`: MySQL服务器地址（默认：localhost）
- `MYSQL_PORT`: MySQL服务器端口（默认：3306）

### 端口配置

- **WebServer**: 50001
- **MySQL**: 3306

### 数据库配置

默认数据库配置：
- 数据库名: `user_info_db`
- 用户名: `webdev`
- 密码: `12345678`

## 日志查看

查看WebServer日志：
```bash
docker-compose logs webserver
```

查看MySQL日志：
```bash
docker-compose logs mysql
```

实时跟踪日志：
```bash
docker-compose logs -f webserver
```

## 数据持久化

- MySQL数据通过Docker Volume持久化存储
- WebServer日志挂载到本地 `./logs` 目录

## 故障排除

1. **端口冲突**:
   如果端口被占用，可以修改 `docker-compose.yml` 中的端口映射

2. **MySQL连接失败**:
   确保MySQL容器完全启动后再启动WebServer。docker-compose配置中已包含健康检查。

3. **查看容器状态**:
   ```bash
   docker ps -a
   ```

4. **进入容器调试**:
   ```bash
   # 进入WebServer容器
   docker exec -it cjj-webserver bash
   
   # 进入MySQL容器
   docker exec -it cjj-webserver-mysql bash
   ```

## 开发模式

如果需要在开发过程中实时查看代码更改：

```bash
# 挂载源代码目录
docker run -d \
  --name cjj-webserver-dev \
  -p 50001:50001 \
  -v $(pwd):/app \
  --link mysql-server:mysql \
  -e MYSQL_HOST=mysql \
  cjj-webserver
```

## 清理

删除所有容器和镜像：
```bash
# 停止并删除容器
docker-compose down

# 删除镜像
docker rmi cjj-webserver

# 清理未使用的Docker资源
docker system prune -a
