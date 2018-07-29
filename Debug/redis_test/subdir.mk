################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../redis_test/adlist.o \
../redis_test/ae.o \
../redis_test/anet.o \
../redis_test/benchmark.o \
../redis_test/dict.o \
../redis_test/hiredis.o \
../redis_test/linenoise.o \
../redis_test/lzf_c.o \
../redis_test/lzf_d.o \
../redis_test/pqsort.o \
../redis_test/queue.o \
../redis_test/redis-cli.o \
../redis_test/redis-stat.o \
../redis_test/redis.o \
../redis_test/sds.o \
../redis_test/zmalloc.o 

C_SRCS += \
../redis_test/adlist.c \
../redis_test/ae.c \
../redis_test/ae_epoll.c \
../redis_test/ae_kqueue.c \
../redis_test/ae_select.c \
../redis_test/anet.c \
../redis_test/benchmark.c \
../redis_test/dict.c \
../redis_test/hiredis.c \
../redis_test/linenoise.c \
../redis_test/lzf_c.c \
../redis_test/lzf_d.c \
../redis_test/pqsort.c \
../redis_test/queue.c \
../redis_test/redis-cli.c \
../redis_test/redis-stat.c \
../redis_test/redis.c \
../redis_test/sds.c \
../redis_test/zmalloc.c 

OBJS += \
./redis_test/adlist.o \
./redis_test/ae.o \
./redis_test/ae_epoll.o \
./redis_test/ae_kqueue.o \
./redis_test/ae_select.o \
./redis_test/anet.o \
./redis_test/benchmark.o \
./redis_test/dict.o \
./redis_test/hiredis.o \
./redis_test/linenoise.o \
./redis_test/lzf_c.o \
./redis_test/lzf_d.o \
./redis_test/pqsort.o \
./redis_test/queue.o \
./redis_test/redis-cli.o \
./redis_test/redis-stat.o \
./redis_test/redis.o \
./redis_test/sds.o \
./redis_test/zmalloc.o 

C_DEPS += \
./redis_test/adlist.d \
./redis_test/ae.d \
./redis_test/ae_epoll.d \
./redis_test/ae_kqueue.d \
./redis_test/ae_select.d \
./redis_test/anet.d \
./redis_test/benchmark.d \
./redis_test/dict.d \
./redis_test/hiredis.d \
./redis_test/linenoise.d \
./redis_test/lzf_c.d \
./redis_test/lzf_d.d \
./redis_test/pqsort.d \
./redis_test/queue.d \
./redis_test/redis-cli.d \
./redis_test/redis-stat.d \
./redis_test/redis.d \
./redis_test/sds.d \
./redis_test/zmalloc.d 


# Each subdirectory must supply rules for building sources it contributes
redis_test/%.o: ../redis_test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


