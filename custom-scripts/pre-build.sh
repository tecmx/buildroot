# !/bin/sh

cp $BASE_DIR/../custom-scripts/S41network-config $BASE_DIR/target/etc/init.d
chmod +x $BASE_DIR/target/etc/init.d/S41network-config

cp $BASE_DIR/../custom-scripts/hello/hello $BASE_DIR/target/usr/bin

cp $BASE_DIR/../T1-LabSisOp/t1-server $BASE_DIR/target/usr/bin

cp $BASE_DIR/../disk-test/test_1 $BASE_DIR/target/usr/bin

cp $BASE_DIR/../custom-scripts/setpriority/deadline $BASE_DIR/target/usr/bin
cp $BASE_DIR/../custom-scripts/setpriority/setpriority $BASE_DIR/target/usr/bin

chmod +x $BASE_DIR/target/usr/bin

make -C $BASE_DIR/../modules/simple_driver/
