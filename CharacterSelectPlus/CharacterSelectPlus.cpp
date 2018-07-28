#include "stdafx.h"
#include "SA2ModLoader.h"
#include <stdio.h>
#include <array>

extern "C"
{
	DataArray(char *, Camera_Character_Data, 0x0173388C, 10);
	DataPointer(char, CharOrMissionSelectionMax, 0x01D1BF04);

	DataArray(char, CheckUnlockedChar, 0x01DEF829, 11);
	DataArray(int, ChaoSelectCharacterIconIDs, 0x00C75310, 6);
	DataPointer(float, StageSelect_MissionCharSelectLoc_X, 0x01A4A968);
	DataPointer(float, StageSelect_MissionCharSelectLoc_Y, 0x01A4A96C);
	DataPointer(float, NJS_IMAGEDATA_AlphaStrength, 0x025EFFD0);
	DataPointer(float, NJS_IMAGEDATA_RedStrength, 0x025EFFD4);
	DataPointer(float, NJS_IMAGEDATA_GreenStrength, 0x025EFFD8);
	DataPointer(float, NJS_IMAGEDATA_BlueStrength, 0x025EFFDC);
	DataArray(NJS_TEXANIM, StageMapAnims, 0x00C68C50, 104);
	DataArray(NJS_TEXNAME, StageMapTexNames, 0x00C68AA8, 16);
	DataArray(CharacterVoiceBank, stru_173A018, 0x0173A018, 10);

	FunctionPointer(void, DisplayMissionRanksName, (int*), 0x00676690);
	FunctionPointer(void, Set_NJSDATA_To, (float), 0x00433D00);
	FunctionPointer(int, DisplayChaoNameEmblems, (int*, float), 0x006768A0);

	//hardcoded values i guess
	DataPointer(float, ToCheck1, 0x01DEB6A8);
	DataPointer(float, ToCheck2, 0x025F0268);
	DataPointer(int, ToCheck3, 0x01DEB6A0);
	DataPointer(int, ToCheck4, 0x025F02D4);

	struct Vertex
	{
		float X;
		float Y;
		float Z;
	};

	bool altCharacter;
	int ChaoWorldCharactersPlus[10] = {
		Characters::Characters_Sonic,
		Characters::Characters_MechTails,
		Characters::Characters_Knuckles,
		Characters::Characters_Shadow,
		Characters::Characters_MechEggman,
		Characters::Characters_Rouge,
		Characters::Characters_Sonic,
		Characters::Characters_Shadow,
		Characters::Characters_Knuckles,
		Characters::Characters_Rouge
	};
	int ConvertCWCP[17] = {
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		0,
		9,
		10,
		11,
		1,
		13,
		14,
		4,
		5
	};
	int ChaoSelectCharacterIconIDsPlus[10] = {
		5,
		6,
		1,
		4,
		0,
		3,
		104,
		105,
		106,
		107
	};
	float IconPosX[10] = {
		102,
		102,
		102,
		102,
		102,
		102,
		302,
		302,
		302,
		302,
	};
	float IconPosY[10] = {
		0,
		0,
		0,
		0,
		0,
		0,
		40,
		40,
		40,
		40
	};
	NJS_TEXANIM NewStageMapAnims[108];
	NJS_TEXNAME NewStageMapTexNames[20];
	

	static const void *const LoadTailsPtr = (void*)0x0074CF00;
	static inline void LoadTailsFixed(int playerNum)
	{
		__asm
		{
			mov eax, [playerNum]
			call LoadTailsPtr
		}
	}

	static const void *const LoadEggmanPtr = (void*)0x0073C220;
	static inline void LoadEggmanFixed(int playerNum)
	{
		__asm
		{
			mov eax, [playerNum]
			call LoadEggmanPtr
		}
	}

	static const void *const CheckSelectedStageTypePtr = (void*)0x00676640;
	static inline void CheckSelectedStageType(int stageID, int UiSelectionID, int *output)
	{
		__asm
		{
			mov eax, [stageID]
			mov edx, [UiSelectionID]
			mov ecx, [output]
			call CheckSelectedStageTypePtr
		}
	}

	static const void *const Display_stageMapTexturePtr = (void*)0x00675C90;
	static inline void Display_stageMapTexture(int UIFlags, Vertex *Position, int TexID, float ESize)
	{
		__asm
		{
			push [ESize]
			push [TexID]
			mov eax, [Position]
			mov ecx, [UIFlags]
			call Display_stageMapTexturePtr
		}
	}

	static const void *const DrawTimeTextPtr = (void*)0x00674F70;
	static inline void DrawTimeText(Vertex *Position, int Time, float ESize)
	{
		__asm
		{
			push [ESize]
			mov esi, [Time]
			mov eax, [Position]
			call DrawTimeTextPtr
		}
	}

	static const void *const DrawScoreTextPtr = (void*)0x00675580;
	static inline void DrawScoreText(Vertex *Position, int number, int CharCount, float ESize)
	{
		__asm
		{
			push [ESize]
			push [CharCount]
			push [number]
			mov eax, [Position]
			call DrawScoreTextPtr
		}
	}

	//the actual mod
	//adjusting the characters to load, so that every character is loadable
	void __cdecl LoadCharactersPlus()
	{
		signed int v0; // esi
		int v1; // eax

		v0 = 0;
		do
		{
			if (!v0)
			{
				switch (CurrentCharacter)
				{
				case Characters::Characters_Sonic:
					if (!altCharacter)
						LoadSonic(0);
					else LoadAmy(0);
					break;
				case Characters::Characters_Shadow:
					if (!altCharacter)
						LoadShadow(0);
					else LoadMetalSonic(0);
					break;
				case Characters::Characters_Knuckles:
					if (!altCharacter)
						LoadKnuckles(0);
					else LoadTikal(0);
					break;
				case Characters::Characters_Rouge:
					if (!altCharacter)
						LoadRouge(0);
					else LoadChaos(0);
					break;
				case Characters::Characters_Tails:
					LoadTailsFixed(0);
					break;
				case Characters::Characters_Eggman:
					LoadEggmanFixed(0);
					break;
				case Characters::Characters_MechEggman:
					LoadMechEggman(0);
					break;
				case Characters::Characters_MechTails:
					LoadMechTails(0);
					break;
				default:
					break;
				}
				v1 = 0;
			LABEL_39:
				InitPlayer(v1);
				goto LABEL_40;
			}
			if (v0 == 1 && TwoPlayerMode)
			{
				if (!CurrentCharacter2P)
					LoadSonic(1);
				if (CurrentCharacter2P == 1)
					LoadShadow(1);
				if (CurrentCharacter2P == 4)
					LoadKnuckles(1);
				if (CurrentCharacter2P == 5)
					LoadRouge(1);
				if (CurrentCharacter2P == 2)
					LoadTailsFixed(1);
				if (CurrentCharacter2P == 3)
					LoadEggmanFixed(1);
				if (CurrentCharacter2P == 7)
					LoadMechEggman(1);
				if (CurrentCharacter2P == 6)
					LoadMechTails(1);
				v1 = 1;
				goto LABEL_39;
			}
		LABEL_40:
			++v0;
		} while (v0 < 2);
	}

	//changing the layout of the character select menu
	void DisplaySelectMissionCharMenu()
	{

		int selectedLevel;
		int selectedStageInfo[3];
		int langIsJap;

		Vertex position;
		Vertex Location_SelectionBorder;

		//variables declaration ends here

		selectedLevel = StageSelectLevels[StageSelectLevelSelection].Level;
		if (selectedLevel == LevelIDs::LevelIDs_GreenHill)
			return;
		CheckSelectedStageType(selectedLevel, StageSelectLevelSelection, selectedStageInfo);

		if (TextLanguage != 0)
			langIsJap = 4;
		else langIsJap = 0;

		position.X = StageSelect_MissionCharSelectLoc_X - 128;
		position.Y = StageSelect_MissionCharSelectLoc_Y - 64;
		position.Z = -95;

		ToCheck1 = ToCheck4;
		ToCheck3 = ToCheck4;
		NJS_IMAGEDATA_AlphaStrength = 0.5;
		NJS_IMAGEDATA_RedStrength = 0.5;
		NJS_IMAGEDATA_GreenStrength = 0.5;
		NJS_IMAGEDATA_BlueStrength = 0.5;

		if (selectedStageInfo[0])
		{
			if (selectedStageInfo[0] == 1)
				Display_stageMapTexture(34, &position, 92, 1);
			else Display_stageMapTexture(34, &position, 93, 1);
		}
		else
		{
			int t;
			switch (SelectedMissionCharacter)
			{
			case 0:
			case 3:
			case 4:
				t = 93;
				break;
			default:
				t = 92;
				break;
			}
			Display_stageMapTexture(34, &position, t, 1);
		}

		ToCheck2 = ToCheck1;
		ToCheck4 = ToCheck3;
		NJS_IMAGEDATA_AlphaStrength = 0.0;
		NJS_IMAGEDATA_RedStrength = 0.0;
		NJS_IMAGEDATA_GreenStrength = 0.0;
		NJS_IMAGEDATA_BlueStrength = 0.0;

		position.X += 55.0;
		position.Y += 140.0;
		position.Z += 1.0;

		switch (selectedStageInfo[0])
		{
		case 0:
			CharOrMissionSelectionMax = 5;
			DisplayMissionRanksName(selectedStageInfo);

			Location_SelectionBorder.X = SelectedMissionCharacter * 50.0 + StageSelect_MissionCharSelectLoc_X - 101.0;
			Location_SelectionBorder.Y = StageSelect_MissionCharSelectLoc_Y + 29.0;
			Location_SelectionBorder.Z = -80;
			Display_stageMapTexture(32, &Location_SelectionBorder, 90, 0.7);
			if (!SelectedMissionCharacter || SelectedMissionCharacter > 2 && SelectedMissionCharacter <= 4)
			{
				altCharacter = false;
				for (int i = 0; i < 4; i++)
				{
					Display_stageMapTexture(32, &position, langIsJap + 94 + i, 1.0);
					position.X += 115.0;
					Display_stageMapTexture(32, &position, 89, 1.0);
					position.X -= 115.0;
					position.Y += 20.0;
				}

				Set_NJSDATA_To(1);

				position.X += 68.0;
				position.Y -= 79.0;
				position.Z += 1.0;
				DrawTimeText(&position, (int)(selectedStageInfo[1] + 36 * SelectedMissionCharacter + 24), 0.6);


				position.X += 97.0;
				position.Y += 20.0;
				DrawScoreText(&position, *(unsigned short*)(selectedStageInfo[1] + 36 * SelectedMissionCharacter + 16), 7, 0.6); // best ring

				position.Y += 20.0;
				DrawScoreText(&position, *(unsigned short*)(selectedStageInfo[1] + 36 * SelectedMissionCharacter + 20), 7, 0.6); // best score

				position.Y += 20.0;
				DrawScoreText(&position, *(unsigned short *)(selectedStageInfo[1] + 2 * SelectedMissionCharacter + 6), 7, 0.6); // no. of plays
				return;
			}
		LABEL_1:
			altCharacter = false;
			for (int i = 0; i < 2; i++)
			{
				Display_stageMapTexture(32, &position, langIsJap + 94 + (i * 3), 1.0);
				position.X += 115.0;
				Display_stageMapTexture(32, &position, 89, 1.0);
				position.X -= 115.0;
				position.Y += 20.0;
			}

			Set_NJSDATA_To(1);

			position.X += 68.0;
			position.Y -= 39.0;
			position.Z += 1.0;

			DrawTimeText(&position, (int)(selectedStageInfo[1] + 36 * SelectedMissionCharacter + 24), 0.6);

			position.X += 97.0;
			position.Y += 20.0;
			DrawScoreText(&position, *(unsigned short*)(selectedStageInfo[1] + 2 * SelectedMissionCharacter + 6), 7, 0.6); //no. of plays

			return;
		case 1:
			CharOrMissionSelectionMax = 5;
			DisplayMissionRanksName(selectedStageInfo);

			Location_SelectionBorder.X = SelectedMissionCharacter * 50.0 + StageSelect_MissionCharSelectLoc_X - 101.0;
			Location_SelectionBorder.Y = StageSelect_MissionCharSelectLoc_Y + 29.0;
			Location_SelectionBorder.Z = -80;
			Display_stageMapTexture(32, &Location_SelectionBorder, 90, 0.7);

			goto LABEL_1;
		case 2:
			CharOrMissionSelectionMax = 10;
			DisplayChaoNameEmblems(selectedStageInfo, 90.0);
			altCharacter = (SelectedMissionCharacter > 5);

			for (int i = 0; i < 10; i++)
			{
				position.X = i * 40.0 + StageSelect_MissionCharSelectLoc_X - IconPosX[i];
				position.Y = StageSelect_MissionCharSelectLoc_Y + IconPosY[i];
				position.Z = -90;

				float IconAlpha;
				if (CheckUnlockedChar[ConvertCWCP[ChaoWorldCharactersPlus[i]]] == 0)
					IconAlpha = 0.5;
				else IconAlpha = 1;

				ToCheck1 = ToCheck2;
				ToCheck3 = ToCheck4;
				NJS_IMAGEDATA_AlphaStrength = 1;
				NJS_IMAGEDATA_RedStrength = 1;
				NJS_IMAGEDATA_GreenStrength = 1;
				NJS_IMAGEDATA_BlueStrength = 1;

				Display_stageMapTexture(34, &position, ChaoSelectCharacterIconIDsPlus[i], 0.6);
				if (SelectedMissionCharacter == i)
				{
					ToCheck1 = ToCheck2;
					ToCheck3 = ToCheck4;
					NJS_IMAGEDATA_AlphaStrength = 1.0;
					NJS_IMAGEDATA_RedStrength = 1.0;
					NJS_IMAGEDATA_GreenStrength = 1.0;
					NJS_IMAGEDATA_BlueStrength = 1.0;

					Display_stageMapTexture(32, &position, 90, 0.6);
				}
			}
			break;
		}

	}
	__declspec(naked) void DisplaySelectMissionCharMenu_Wrapper()
	{
		__asm
		{
			push esi
			push ebx
			push edi
			push ecx
			call DisplaySelectMissionCharMenu
			pop ecx
			pop edi
			pop ebx
			pop esi
			ret
		}
	}

	// fixing the sound loading
	ThiscallFunctionPointer(void, sub_435880, (char *), 0x435880);
	void LoadChaoWorldSoundBank()
	{
		char *v4;
		int v2 = CurrentCharacter;
		if (altCharacter)
		{
			switch (CurrentCharacter)
			{
			case 0:
				v2 = Characters::Characters_Amy;
				break;
			case 1:
				v2 = Characters::Characters_MetalSonic;
				break;
			case 4:
				v2 = Characters::Characters_Tikal;
				break;
			case 5:
				v2 = Characters::Characters_Chaos;
				break;
			default:
				break;
			}
		}

		switch (v2)
		{
		case Characters_Sonic:
		case Characters_Shadow:
		case Characters_Amy:
		case Characters_MetalSonic:
			v4 = (char*)"chao_chara_ss.mlt";
			break;
		case Characters_Knuckles:
		case Characters_Rouge:
			v4 = (char*)"chao_chara_kr.mlt";
			break;
		case Characters_Tikal:
		case Characters_Chaos:
			v4 = (char*)"se_ch_kn_BATTLE.mlt";
			break;
		case Characters_Tails:
		case Characters_Eggman:
			v4 = (char*)"chao_chara_te.mlt";
			break;
		case Characters_MechTails:
		case Characters_MechEggman:
			v4 = (char*)"se_ch_wk.mlt";
			break;
		case Characters_ChaoWalker:
		case Characters_DarkChaoWalker:
			v4 = (char*)"se_ch_wk_BATTLE.mlt";
			break;
		default:
			v4 = (char*)"chao_chara_ss.mlt";
			break;
		}
		sub_435880(v4);
	}

	static const int loc_532054 = 0x532054;
	__declspec(naked) void loc_532029()
	{
		LoadChaoWorldSoundBank();
		__asm jmp loc_532054
	}

	DataArray(CharacterVoiceBank, stru_173A0B8, 0x173A0B8, 7);
	FunctionPointer(void, sub_459010, (int, CharacterVoiceBank*), 0x459010);
	static const int loc_45923B = 0x45923B;
	__declspec(naked) void loc_2800440()
	{
		__asm
		{
			mov ecx, CurrentCharacter
			mov ecx, [ecx]
			cmp altCharacter, 0
			je done
			cmp ecx, Characters_Sonic
			jne notAmy
				mov ecx, Characters_Amy
				mov	esi, 173A0B8h
			jmp done
		notAmy :
			cmp ecx, Characters_Shadow
			jne notMetal
				mov ecx, Characters_MetalSonic
				mov	esi, 173A0B8h + 10h
			jmp done
		notMetal :
			cmp ecx, Characters_Knuckles
			jne notKnuckles
				mov ecx, Characters_Tikal
				mov	esi, 173A0B8h + 20h
			jmp done
		notKnuckles :
			cmp ecx, Characters_Rouge
			jne done
				mov ecx, Characters_Chaos
				mov	esi, 173A0B8h + 30h

		done :
			call	sub_459010
			jmp	loc_45923B
		}
	}

	//inserting the mods
	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		std::memcpy(NewStageMapAnims, StageMapAnims, sizeof(NJS_TEXANIM) * 104); // copy the texanims
		//std::memcpy()
		NewStageMapAnims[104] = { 0x3B, 0x3B, 0, 0, 8, 8, 0x0F4, 0x0F4, 16, 0 }; // add the new textures
		NewStageMapAnims[105] = { 0x3B, 0x3B, 0, 0, 8, 8, 0x0F4, 0x0F4, 17, 0 };
		NewStageMapAnims[106] = { 0x3B, 0x3B, 0, 0, 8, 8, 0x0F4, 0x0F4, 18, 0 };
		NewStageMapAnims[107] = { 0x3B, 0x3B, 0, 0, 8, 8, 0x0F4, 0x0F4, 19, 0 };

		WriteData((int*)0x00675CBB, (int)&NewStageMapAnims); // re-assign the texanims
		WriteData((int*)0x00675D20, (int)&NewStageMapAnims);
		WriteData((int*)0x006765F6, (int)&NewStageMapAnims);
		WriteData((int*)0x006766B2, (int)&NewStageMapAnims);
		WriteData((int*)0x006769B5, (int)&NewStageMapAnims);
		WriteData((int*)0x00676A0E, (int)&NewStageMapAnims);
		WriteData((int*)0x00677A53, (int)&NewStageMapAnims->cx);
		WriteData((int*)0x00677A8B, (int)&NewStageMapAnims[108].cx);
		WriteData((int*)0x00677A8B, (int)&NewStageMapAnims[108].cx);
		WriteData((int*)0x00C68B6C, 0x14); // raise spritecount to 20
		WriteCall((int*)0x0067792F, DisplaySelectMissionCharMenu_Wrapper);

		WriteJump((void*)0x0043D630, LoadCharactersPlus);
		WriteData((int*)0x006784A0, (int)&ChaoWorldCharactersPlus);
		WriteData((int*)0x006782A6, (int)&ChaoWorldCharactersPlus);
		WriteJump((void*)0x532029, loc_532029);
		WriteJump((void*)0x459236, loc_2800440);
	}
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}
