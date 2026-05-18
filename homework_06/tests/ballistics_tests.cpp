#include <gtest/gtest.h>
#include "../include/ballistics.hpp"
#include "../src/ballistics.cpp"



TEST (readBallisticFileTest, HandlesCorrectFile) {
    BallisticParams params = readBallisticFile("../data/sample_vog17.txt");
    EXPECT_FLOAT_EQ(params.xd, 100.0);
    EXPECT_FLOAT_EQ(params.yd, 50.0);
    EXPECT_FLOAT_EQ(params.zd, 20.0);
    EXPECT_FLOAT_EQ(params.targetX, 150.0);
    EXPECT_FLOAT_EQ(params.targetY, 75.0);
    EXPECT_FLOAT_EQ(params.attackSpeed, 300.0);
    EXPECT_FLOAT_EQ(params.accelerationPath, 10.0);
    EXPECT_EQ(params.ammo_name, "RKG-3");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}