#include "global.h"
#include "battle_pyramid.h"
#include "bg.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "menu.h"
#include "palette.h"
#include "start_menu.h"
#include "string_util.h"
#include "sound.h"
#include "task.h"
#include "text.h"
#include "constants/battle_frontier.h"
#include "constants/layouts.h"
#include "constants/region_map_sections.h"
#include "constants/weather.h"
#include "constants/songs.h"

#include "rogue.h"
#include "rogue_campaign.h"
#include "rogue_controller.h"
#include "rogue_popup.h"

#define POPUP_QUEUE_CAPACITY 8

struct PopupRequest
{
    u16 param;
    u8 msgType;
};

static void ShowQuestPopup(void);
static void HideQuestPopUpWindow(void);

static void Task_QuestPopUpWindow(u8 taskId);
static void ShowQuestPopUpWindow(void);
static void LoadQuestPopUpWindowBg(void);

static EWRAM_DATA u8 sPopupTaskId = 0;
static EWRAM_DATA u8 sPopupShownId = 0;
static EWRAM_DATA u8 sPopupQueuedId = 0;
static EWRAM_DATA struct PopupRequest sPopupQueue[POPUP_QUEUE_CAPACITY];

extern const u8 gText_Space[];

extern const u8 gText_Popup_QuestComplete[];
extern const u8 gText_Popup_QuestFail[];
extern const u8 gText_Popup_LegendaryClause[];
extern const u8 gText_Popup_None[];

extern const u8 gPopupText_WeakLegendaryClause[];
extern const u8 gPopupText_StrongLegendaryClause[];

extern const u8 gPopupText_CampaignNoneScore[];
extern const u8 gPopupText_CampaignHighScore[];
extern const u8 gPopupText_CampaignLowScore[];

extern const u8 gPopupText_SafariArea[];
extern const u8 gPopupText_StarterWarning[];


static const u16 sQuestPopupMessageSoundEffect[] =
{
    [POPUP_MSG_QUEST_COMPLETE] = SE_EXP_MAX,
    [POPUP_MSG_QUEST_FAIL] = SE_NOT_EFFECTIVE,
    [POPUP_MSG_LEGENDARY_CLAUSE] = SE_BALL_OPEN,
    [POPUP_MSG_CAMPAIGN_ANNOUNCE] = SE_EXP_MAX,
    [POPUP_MSG_SAFARI_ENCOUNTERS] = 0,
    [POPUP_MSG_PARTNER_EVO_WARNING] = SE_NOT_EFFECTIVE,
    //SE_SUCCESS, SE_FAILURE
};

static void ShowQuestPopup(void)
{
    if (!FuncIsActiveTask(Task_QuestPopUpWindow))
    {
        sPopupTaskId = CreateTask(Task_QuestPopUpWindow, 90);
        SetGpuReg(REG_OFFSET_BG0VOFS, 40);
        gTasks[sPopupTaskId].data[0] = 6;
        gTasks[sPopupTaskId].data[2] = 40;
    }
    else
    {
        if (gTasks[sPopupTaskId].data[0] != 2)
            gTasks[sPopupTaskId].data[0] = 2;
        gTasks[sPopupTaskId].data[3] = 1;
    }
}

void Rogue_PushPopup(u8 msgType, u16 param)
{
    // Write to the current ID and then push
    sPopupQueue[sPopupQueuedId].msgType = msgType;
    sPopupQueue[sPopupQueuedId].param = param;
    sPopupQueuedId = (sPopupQueuedId + 1) % POPUP_QUEUE_CAPACITY;
}

void Rogue_ClearPopupQueue(void)
{
    if (FuncIsActiveTask(Task_QuestPopUpWindow))
        HideQuestPopUpWindow();

    sPopupQueuedId = 0;
    sPopupShownId = 0;
}

void Rogue_UpdatePopups(bool8 inOverworld, bool8 inputEnabled)
{
    bool8 enabled = inOverworld && inputEnabled; // May need to check this too? GetStartMenuWindowId

    if(enabled)
    {
        if(sPopupQueuedId != sPopupShownId)
        {
            if (!FuncIsActiveTask(Task_QuestPopUpWindow))
                ShowQuestPopup();
        }
    }
    else
    {
        if (FuncIsActiveTask(Task_QuestPopUpWindow))
            HideQuestPopUpWindow();
    }
}

static void Task_QuestPopUpWindow(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 6:
        task->data[4]++;
        if (task->data[4] > 5)
        {
            task->data[0] = 0;
            task->data[4] = 0;
            ShowQuestPopUpWindow();
        }
        break;
    case 0:
        task->data[2] -= 2;
        if (task->data[2] <= 0 )
        {
            task->data[2] = 0;
            task->data[0] = 1;
            gTasks[sPopupTaskId].data[1] = 0;
        }
        break;
    case 1:
        task->data[1]++;
        if (task->data[1] > 120 )
        {
            task->data[1] = 0;
            task->data[0] = 2;
        }
        break;
    case 2:
        task->data[2] += 2;
        if (task->data[2] > 39)
        {
            task->data[2] = 40;
            if (task->data[3])
            {
                task->data[0] = 6;
                task->data[4] = 0;
                task->data[3] = 0;
            }
            else
            {
                task->data[0] = 4;
                return;
            }
        }
        break;
    case 4:
        ClearStdWindowAndFrame(GetQuestPopUpWindowId(), TRUE);
        task->data[0] = 5;
        break;
    case 5:
        HideQuestPopUpWindow();
        return;
    }
    SetGpuReg(REG_OFFSET_BG0VOFS, task->data[2]);
}

static void HideQuestPopUpWindow(void)
{
    if (FuncIsActiveTask(Task_QuestPopUpWindow))
    {
        ClearStdWindowAndFrame(GetQuestPopUpWindowId(), TRUE);
        RemoveQuestPopUpWindow();
        SetGpuReg_ForcedBlank(REG_OFFSET_BG0VOFS, 0);
        DestroyTask(sPopupTaskId);
        
        sPopupShownId = (sPopupShownId + 1) % POPUP_QUEUE_CAPACITY;
    }
}

const u8* GetMsgText(u8 msgType)
{
    switch (msgType)
    {
    case POPUP_MSG_QUEST_COMPLETE:
        return &gText_Popup_QuestComplete[0];

    case POPUP_MSG_QUEST_FAIL:
        return &gText_Popup_QuestFail[0];

    case POPUP_MSG_LEGENDARY_CLAUSE:
        return &gText_Popup_LegendaryClause[0];
    
    default:
        return &gText_Popup_None[0];
    }
}

static u8* AppendTypeName(u8* strPointer, u8 type)
{
    const u8 gText_AdjNormal[] = _("NORMAL");
    const u8 gText_AdjFighting[] = _("FIGHT"); // Shortened for convienience
    const u8 gText_AdjFlying[] = _("FLYING");
    const u8 gText_AdjPoison[] = _("POISON");
    const u8 gText_AdjGround[] = _("GROUND");
    const u8 gText_AdjRock[] = _("ROCK");
    const u8 gText_AdjBug[] = _("BUG");
    const u8 gText_AdjGhost[] = _("GHOST");
    const u8 gText_AdjSteel[] = _("STEEL");
    const u8 gText_AdjFire[] = _("FIRE");
    const u8 gText_AdjWater[] = _("WATER");
    const u8 gText_AdjGrass[] = _("GRASS");
    const u8 gText_AdjElectric[] = _("ELEC"); // Shortened for convienience
    const u8 gText_AdjPsychic[] = _("PSYCHIC");
    const u8 gText_AdjIce[] = _("ICE");
    const u8 gText_AdjDragon[] = _("DRAGON");
    const u8 gText_AdjDark[] = _("DARK");
#ifdef ROGUE_EXPANSION
    const u8 gText_AdjFairy[] = _("FAIRY");
#endif
    const u8 gText_AdjNone[] = _("???");

    switch(type)
    {
        case TYPE_NORMAL:
            return StringAppend(strPointer, gText_AdjNormal);

        case TYPE_FIGHTING:
            return StringAppend(strPointer, gText_AdjFighting);

        case TYPE_FLYING:
            return StringAppend(strPointer, gText_AdjFlying);

        case TYPE_POISON:
            return StringAppend(strPointer, gText_AdjPoison);

        case TYPE_GROUND:
            return StringAppend(strPointer, gText_AdjGround);

        case TYPE_ROCK:
            return StringAppend(strPointer, gText_AdjRock);

        case TYPE_BUG:
            return StringAppend(strPointer, gText_AdjBug);

        case TYPE_GHOST:
            return StringAppend(strPointer, gText_AdjGhost);

        case TYPE_STEEL:
            return StringAppend(strPointer, gText_AdjSteel);

        case TYPE_FIRE:
            return StringAppend(strPointer, gText_AdjFire);

        case TYPE_WATER:
            return StringAppend(strPointer, gText_AdjWater);

        case TYPE_GRASS:
            return StringAppend(strPointer, gText_AdjGrass);

        case TYPE_ELECTRIC:
            return StringAppend(strPointer, gText_AdjElectric);

        case TYPE_PSYCHIC:
            return StringAppend(strPointer, gText_AdjPsychic);

        case TYPE_ICE:
            return StringAppend(strPointer, gText_AdjIce);

        case TYPE_DRAGON:
            return StringAppend(strPointer, gText_AdjDragon);

        case TYPE_DARK:
            return StringAppend(strPointer, gText_AdjDark);

#ifdef ROGUE_EXPANSION
        case TYPE_FAIRY:
            return StringAppend(strPointer, gText_AdjFairy);
#endif

        default:
            return StringAppend(strPointer, gText_AdjNone);
    }
}

static void ShowQuestPopUpWindow(void)
{
    u8 x;
    u16 param = sPopupQueue[sPopupShownId].param;
    u8 msgType = sPopupQueue[sPopupShownId].msgType;

    AddQuestPopUpWindow();

    PutWindowTilemap(GetQuestPopUpWindowId());
    DrawStdWindowFrame(GetQuestPopUpWindowId(), FALSE);

    switch (msgType)
    {
    case POPUP_MSG_QUEST_COMPLETE:
    case POPUP_MSG_QUEST_FAIL:
        // Quest Title
        x = GetStringCenterAlignXOffset(FONT_NARROW, gRogueQuests[param].title, 80);
        AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_NARROW, gRogueQuests[param].title, x, 2, TEXT_SKIP_DRAW, NULL);

        // Msg Subtext
        x = GetStringCenterAlignXOffset(FONT_SMALL, GetMsgText(msgType), 80);
        AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_SMALL, GetMsgText(msgType), x, 16, TEXT_SKIP_DRAW, NULL);
        break;

    case POPUP_MSG_LEGENDARY_CLAUSE:
        // Legendary Clause Title
        if(param == 0)
        {
            x = GetStringCenterAlignXOffset(FONT_NARROW, gPopupText_WeakLegendaryClause, 80);
            AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_NARROW, gPopupText_WeakLegendaryClause, x, 2, TEXT_SKIP_DRAW, NULL);
        }
        else
        {
            x = GetStringCenterAlignXOffset(FONT_NARROW, gPopupText_StrongLegendaryClause, 80);
            AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_NARROW, gPopupText_StrongLegendaryClause, x, 2, TEXT_SKIP_DRAW, NULL);
        }

        // Msg Subtext
        x = GetStringCenterAlignXOffset(FONT_SMALL, GetMsgText(msgType), 80);
        AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_SMALL, GetMsgText(msgType), x, 16, TEXT_SKIP_DRAW, NULL);
        break;

    case POPUP_MSG_CAMPAIGN_ANNOUNCE:
        // Campaign Title
        x = GetStringCenterAlignXOffset(FONT_NARROW, GetCampaignTitle(Rogue_GetActiveCampaign()), 80);
        AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_NARROW, GetCampaignTitle(Rogue_GetActiveCampaign()), x, 2, TEXT_SKIP_DRAW, NULL);

        if(Rogue_IsActiveCampaignScored())
        {
            if(Rogue_IsActiveCampaignLowScoreGood())
            {
                x = GetStringCenterAlignXOffset(FONT_SMALL, gPopupText_CampaignLowScore, 80);
                AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_SMALL, gPopupText_CampaignLowScore, x, 16, TEXT_SKIP_DRAW, NULL);
            }
            else
            {
                x = GetStringCenterAlignXOffset(FONT_SMALL, gPopupText_CampaignHighScore, 80);
                AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_SMALL, gPopupText_CampaignHighScore, x, 16, TEXT_SKIP_DRAW, NULL);
            }
        }
        else
        {
            x = GetStringCenterAlignXOffset(FONT_SMALL, gPopupText_CampaignNoneScore, 80);
            AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_SMALL, gPopupText_CampaignNoneScore, x, 16, TEXT_SKIP_DRAW, NULL);
        }
        break;

    case POPUP_MSG_SAFARI_ENCOUNTERS:
        {
            u8 i;
            u8* strPointer = &gStringVar4[0];
            u8 types[3];

            Rogue_SafariTypeForMap(&types[0], ARRAY_COUNT(types));

            *strPointer = EOS;

            for(i = 0; i < ARRAY_COUNT(types); ++i)
            {
                if(types[i] != TYPE_NONE)
                {
                    if(i != 0)
                        strPointer = StringAppend(strPointer, gText_Space);

                    strPointer = AppendTypeName(strPointer, types[i]);
                }
            }

            // Title contains types
            x = GetStringCenterAlignXOffset(FONT_NARROW, gPopupText_SafariArea, 80);
            AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_NARROW, gPopupText_SafariArea, x, 2, TEXT_SKIP_DRAW, NULL);

            // Subheader
            x = GetStringCenterAlignXOffset(FONT_SMALL, gStringVar4, 80);
            AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_SMALL, gStringVar4, x, 16, TEXT_SKIP_DRAW, NULL);
        }
        break;

    case POPUP_MSG_PARTNER_EVO_WARNING:
        // Title contains entire warning
        x = GetStringCenterAlignXOffset(FONT_NARROW, gPopupText_StarterWarning, 80);
        AddTextPrinterParameterized(GetQuestPopUpWindowId(), FONT_NARROW, gPopupText_StarterWarning, x, 0, TEXT_SKIP_DRAW, NULL);
        break;

    default:
        break;
    }

    CopyWindowToVram(GetQuestPopUpWindowId(), COPYWIN_FULL);

    if(sQuestPopupMessageSoundEffect[msgType])
        PlaySE(sQuestPopupMessageSoundEffect[msgType]);
}
