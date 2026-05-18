#include <gtest/gtest.h>
#include "../include/ballistics.hpp"
#include "../src/ballistics.cpp"



TEST (readBallisticFileTest, HandlesCorrectFile) {
    BallisticParams params = readBallisticFile("../data/sample_vog17.txt");
    EXPECT_FLOAT_EQ(params.xd, 100.0);
    EXPECT_FLOAT_EQ(params.yd, 100.0);
    EXPECT_FLOAT_EQ(params.zd, 100.0);
    EXPECT_FLOAT_EQ(params.targetX, 200.0);
    EXPECT_FLOAT_EQ(params.targetY, 200.0);
    EXPECT_FLOAT_EQ(params.attackSpeed, 10.0);
    EXPECT_FLOAT_EQ(params.accelerationPath, 10.0);
    EXPECT_STREQ(params.ammo_name, "VOG-17");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}