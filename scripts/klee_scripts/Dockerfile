FROM klee/klee:latest 

WORKDIR /home/klee/

RUN sudo apt-get update || true
RUN sudo apt-get install -y time libmbedtls-dev cmake bison flex libboost-all-dev python perl zlib1g-dev minisat libunwind8-dev autoconf
RUN sudo apt-get install -y build-essential cmake curl file g++-multilib gcc-multilib git libcap-dev libgoogle-perftools-dev libncurses5-dev libsqlite3-dev libtcmalloc-minimal4 python3-pip unzip graphviz doxygen
RUN pip3 install z3 z3-solver

RUN git clone https://github.com/stp/stp.git
RUN cd stp && git checkout tags/2.3.3 && mkdir build && cd build  && cmake .. && make && sudo make install

RUN git clone https://github.com/gperftools/gperftools.git
RUN cd gperftools && sh autogen.sh && ./configure && make all && sudo make install

COPY --chown=klee:klee klee_scripts/klee_mod/Executor.cpp /home/klee/klee_src/lib/Core/Executor.cpp
COPY --chown=klee:klee klee_scripts/klee_mod/Executor.h /home/klee/klee_src/lib/Core/Executor.h
COPY --chown=klee:klee klee_scripts/klee_mod/Executor.cpp /home/klee/klee_src/lib/Core/Executor.cpp

WORKDIR /home/klee/klee_src
RUN mkdir build && cd build \
    && cmake -DENABLE_SOLVER_STP=ON -DSTP_DIR=/tmp/stp-2.3.3-install/lib/cmake/STP -DENABLE_POSIX_RUNTIME=ON -DKLEE_UCLIBC_PATH=/tmp/klee-uclibc-110 .. \
    && make -j4

WORKDIR /home/klee/hedb 

COPY --chown=klee:klee . /home/klee/hedb
COPY --chown=klee:klee klee_scripts/logs /home/klee/001-log

RUN make configure_sim && make 
RUN make klee_wrapper

# CMD ["/bin/bash"]
CMD ["/home/klee/hedb/klee_scripts/run_test.sh", "test.log"]