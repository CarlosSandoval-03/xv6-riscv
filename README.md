# XV6

## Getting started

First make a copy of this repository using the following command (this repository is a fork of the [official xv6-riscv repository](https://github.com/mit-pdos/xv6-riscv)):

```shell
git clone git@github.com:CarlosSandoval-03/xv6-riscv.git
```

Now download the docker image `wtakuo/xv6-env`, using the command:

```shell
docker pull wtakuo/xv6-env
```

Add execute permissions to the bash script, and run it

```shell
chmod +x start_container.sh
./start_container.sh
```

If things go well, you should see the following output from the newly started container.

```text
To run a command as administrator (user "root"), use "sudo <command>".
See "man sudo_root" for details.

xv6@f5f11f817173:~/xv6-riscv$
```

Now build xv6 and run it

```shell
make # build
make qemu # run
```

If all goes well you will see something similar to:

```text
xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
```

**NOTE:** To exit the qemu simulation, type `ctrl-A` followed by `x`. To exit the environment, enter the command `exit`

## References

1. [Official xv6-riscv repository](https://github.com/mit-pdos/xv6-riscv)
2. [Docker image of the environment used](https://hub.docker.com/r/wtakuo/xv6-env)
