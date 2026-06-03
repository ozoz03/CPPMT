#include <gtest/gtest.h>

#include "ballistics.hpp"

TEST(readBallisticFileTest, HandlesCorrectFile) {
    BallisticParams params = readBallisticFile("./data/sample_vog17.txt");
    EXPECT_FLOAT_EQ(params.xd, 100.0);
    EXPECT_FLOAT_EQ(params.yd, 100.0);
    EXPECT_FLOAT_EQ(params.zd, 100.0);
    EXPECT_FLOAT_EQ(params.targetX, 200.0);
    EXPECT_FLOAT_EQ(params.targetY, 200.0);
    EXPECT_FLOAT_EQ(params.attackSpeed, 10.0);
    EXPECT_FLOAT_EQ(params.accelerationPath, 10.0);
    EXPECT_STREQ(std::string(params.ammo_name.data()).c_str(), "VOG-17");
}

TEST(getAmmoParamsTest, HandlesKnownAmmo) {
    AmmoParams ammo = getAmmoParams("VOG-17");
    EXPECT_FLOAT_EQ(ammo.mass, 0.35);
    EXPECT_FLOAT_EQ(ammo.drag, 0.07);
    EXPECT_FLOAT_EQ(ammo.lift, 0.0);
    EXPECT_EQ(ammo.type, 0);
}

TEST(getAmmoParamsTestWithWrongName, HandlesUnknownAmmo) {
    EXPECT_DEATH(getAmmoParams("Unknown"), "Error: Wrong ammo_name.");
}

TEST(getTimeByCardanoTest, HandlesValidInput) {
    AmmoParams bomb = {"VOG-17", 0.37, 0.07, 0};
    BallisticParams params = {150.0, 150.0, 100.0, 0.0, 0.0, 10.0, 10.0, "VOG-17"};
    float time = getTimeByCardano(bomb, params);
    EXPECT_FLOAT_EQ(time, 5.61941624);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}