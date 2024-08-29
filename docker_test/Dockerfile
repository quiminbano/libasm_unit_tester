FROM --platform=linux/amd64 alpine:3.20

RUN echo "y" | apk add alpine-sdk nasm bash valgrind \
clang

WORKDIR /app

COPY . .

WORKDIR /app/libasm_unit_tester

CMD ["bash"]