#include "constants/layouts.h"
#include "constants/maps.h"

#define ROUTE_MAP(map) { .layout=LAYOUT_##map, .group=MAP_GROUP(map), .num=MAP_NUM(map), }

static const u8 sRouteEncontersField[] = {
    TYPE_GRASS, TYPE_NORMAL
};
static const struct RogueRouteMap sRouteMapsField[] = {
    ROUTE_MAP(ROGUE_ROUTE_FIELD0), ROUTE_MAP(ROGUE_ROUTE_FIELD1)
};

static const u8 sRouteEncontersForest[] = {
    TYPE_BUG, TYPE_GHOST, TYPE_POISON
};
static const struct RogueRouteMap sRouteMapsForest[] = {
    ROUTE_MAP(ROGUE_ROUTE_FOREST0)
};

static const u8 sRouteEncontersCave[] = {
    TYPE_ROCK, TYPE_ICE, TYPE_DRAGON
};
static const struct RogueRouteMap sRouteMapsCave[] = {
    ROUTE_MAP(ROGUE_ROUTE_CAVE0)
};

static const u8 sRouteEncontersMountain[] = {
    TYPE_GROUND, TYPE_FIRE, TYPE_FIGHTING
};
static const struct RogueRouteMap sRouteMapsMountain[] = {
    ROUTE_MAP(ROGUE_ROUTE_MOUNTAIN0)
};

static const u8 sRouteEncontersWaterfront[] = {
    TYPE_WATER, TYPE_FLYING
};
static const struct RogueRouteMap sRouteMapsWaterfront[] = {
    ROUTE_MAP(ROGUE_ROUTE_WATER_FRONT0)
};

static const u8 sRouteEncontersUrban[] = {
    TYPE_STEEL, TYPE_ELECTRIC, TYPE_PSYCHIC
};
static const struct RogueRouteMap sRouteMapsUrban[] = {
    ROUTE_MAP(ROGUE_ROUTE_URBAN0)
};

const struct RogueRouteData gRogueRouteTable[ROGUE_ROUTE_COUNT] = {
    [ROGUE_ROUTE_FIELD] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersField),
        .wildTypeTable = sRouteEncontersField,
        .mapCount = ARRAY_COUNT(sRouteMapsField),
        .mapTable = sRouteMapsField
    },
    [ROGUE_ROUTE_FOREST] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersForest),
        .wildTypeTable = sRouteEncontersForest,
        .mapCount = ARRAY_COUNT(sRouteMapsForest),
        .mapTable = sRouteMapsForest
    },
    [ROGUE_ROUTE_CAVE] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersCave),
        .wildTypeTable = sRouteEncontersCave,
        .mapCount = ARRAY_COUNT(sRouteMapsCave),
        .mapTable = sRouteMapsCave
    },
    [ROGUE_ROUTE_MOUNTAIN] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersMountain),
        .wildTypeTable = sRouteEncontersMountain,
        .mapCount = ARRAY_COUNT(sRouteMapsMountain),
        .mapTable = sRouteMapsMountain
    },
    [ROGUE_ROUTE_WATERFRONT] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersWaterfront),
        .wildTypeTable = sRouteEncontersWaterfront,
        .mapCount = ARRAY_COUNT(sRouteMapsWaterfront),
        .mapTable = sRouteMapsWaterfront
    },
    [ROGUE_ROUTE_URBAN] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersUrban),
        .wildTypeTable = sRouteEncontersUrban,
        .mapCount = ARRAY_COUNT(sRouteMapsUrban),
        .mapTable = sRouteMapsUrban
    },
};