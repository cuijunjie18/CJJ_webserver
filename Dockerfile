# 使用最新的Ubuntu镜像作为基础镜像
# FROM ubuntu:latest

# 实习mmt镜像
FROM artifactory.momenta.works/docker/ubuntu:22.04

# 设置非交互式安装，避免apt安装时的提示
ENV DEBIAN_FRONTEND=noninteractive

# 设置工作目录
WORKDIR /app

# 复制项目文件到容器中
COPY . /app/

# 更新软件包列表并安装必要的依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libmysqlclient-dev \
    mysql-client \
    && rm -rf /var/lib/apt/lists/*

# 创建构建目录并编译项目
RUN make clean && make build

# 创建日志目录
RUN mkdir -p /app/logs

# 暴露端口 (根据config.hpp中的默认端口50001)
EXPOSE 50001

# 设置启动命令
CMD ["./build/bin/main"]
