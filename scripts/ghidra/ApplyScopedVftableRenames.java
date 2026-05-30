// @category bulanci
import ghidra.app.script.GhidraScript;
import ghidra.app.util.NamespaceUtils;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.SymbolTable;

public class ApplyScopedVftableRenames extends GhidraScript {
    private static final String[][] VFT = {
        {"0x00481a6c", "g_pCGame_vftable_IDSEventHandler"},
        {"0x00481a80", "g_pCGame_vftable_IDSChained"},
        {"0x00481a98", "g_pCGame_vftable_IDSReferenced"},
        {"0x00480f40", "g_pCGameCounter_vftable_IDSEventHandler"},
        {"0x00480f54", "g_pCGameCounter_vftable_IDSUpdated"},
        {"0x00480f6c", "g_pCGameCounter_vftable_IDSChained"},
        {"0x00480f8c", "g_pCGameCounter_vftable_primary"},
        {"0x00482704", "g_pCGaming_vftable_IDSEventHandler_0x10"},
        {"0x0048271c", "g_pCGaming_vftable_IDSReferenced_0x18"},
        {"0x00482730", "g_pCGaming_vftable_IDSUpdated_0x68"},
        {"0x00482748", "g_pCGaming_vftable_IDSChained"},
        {"0x00482764", "g_pCGaming_vftable_primary"},
        {"0x00481d6c", "g_pCLevelScript_vftable_IDSChained"},
        {"0x00481d84", "g_pCLevelScript_vftable_IDSChained_481d84"},
        {"0x00481d9c", "g_pCLevelScript_vftable_IDSEventHandler"},
        {"0x00481db0", "g_pCLevelScript_vftable_IDSChained_481db0"},
        {"0x00481dcc", "g_pCLevelScript_vftable"},
        {"0x00483980", "g_pCNumCounter_vftable_IDSEventHandler_0x10"},
        {"0x00483998", "g_pCNumCounter_vftable_IDSReferenced_0x18"},
        {"0x004839ac", "g_pCNumCounter_vftable_IDSUpdated_0x68"},
        {"0x004839c4", "g_pCNumCounter_vftable_IDSChained"},
        {"0x004839e4", "g_pCNumCounter_vftable_primary"},
        {"0x00481000", "g_pCScore_vftable_IDSEventHandler_0x10"},
        {"0x00481014", "g_pCScore_vftable_IDSReferenced_0x18"},
        {"0x0048102c", "g_pCScore_vftable_IDSChained"},
        {"0x0048104c", "g_pCScore_vftable_primary"},
    };
    private static final String[][] FN = {
        {"0x004028a0", "CBulanci::CDSView_HitTest"},
        {"0x004033a0", "CDSApp::CDSView_GetDataKindStubZero"},
        {"0x004055c0", "CWindow::CWindow_Render"},
        {"0x00409470", "CPoem::GetResourceName"},
        {"0x00409480", "CPoem::AlwaysReturnsZero"},
        {"0x0040abe0", "CScore::OnKeyPress"},
        {"0x0040b140", "CGameCounter::OnEvent"},
        {"0x0040bca0", "CGameCounter::GetClassIdentifier"},
        {"0x0040bcb0", "CGameCounter::DeletingDestructorThunk_10"},
        {"0x0040bcc0", "CGameCounter::DeletingDestructorThunk_18"},
        {"0x0040bcd0", "CGameCounter::DeletingDestructorThunk_4"},
        {"0x0040bdd0", "CScore::GetClassIdentifier"},
        {"0x0040bde0", "CScore::DeletingDestructorThunk_18"},
        {"0x0040bdf0", "CScore::DeletingDestructorThunk_4"},
        {"0x0040be00", "CScore::DeletingDestructorThunk_10"},
        {"0x0040ef40", "CGameCounter::ScalarDeletingDestructor"},
        {"0x0040f070", "CScore::ScalarDeletingDestructor"},
        {"0x00414c60", "CGame::FUN_00414c60"},
        {"0x00414c70", "CGame::FUN_00414c70"},
        {"0x00414c80", "CGame::FUN_00414c80"},
        {"0x00414c90", "CGame::FUN_00414c90"},
        {"0x00415f60", "CGame::FUN_00415f60"},
        {"0x00416030", "CGame::CGaming_SchedulerPump"},
        {"0x00416770", "CBulanci::CDSView_OnMouseStub"},
        {"0x004167d0", "CDSApp::CDSView_RenderChildrenClipped"},
        {"0x00416860", "CGaming::FUN_00416860"},
        {"0x004168a0", "CGaming::FUN_004168a0"},
        {"0x00416a90", "CLevelScript::CLevelScriptTimer_FireOnTimer"},
        {"0x00418680", "CLevelScript::FUN_00418680"},
        {"0x00418690", "CLevelScript::FUN_00418690"},
        {"0x004186a0", "CLevelScript::FUN_004186a0"},
        {"0x004186b0", "CLevelScript::FUN_004186b0"},
        {"0x004186c0", "CLevelScript::FUN_004186c0"},
        {"0x004186d0", "CLevelScript::FUN_004186d0"},
        {"0x004186e0", "CLevelScript::FUN_004186e0"},
        {"0x00418be0", "CLevelScript::FUN_00418be0"},
        {"0x00418bf0", "CLevelScript::FUN_00418bf0"},
        {"0x0041a450", "CLevelScript::FUN_0041a450"},
        {"0x0041aca0", "CGaming::FUN_0041aca0"},
        {"0x0041acb0", "CGaming::FUN_0041acb0"},
        {"0x0041acc0", "CGaming::FUN_0041acc0"},
        {"0x0041acd0", "CGaming::FUN_0041acd0"},
        {"0x0041ace0", "CGaming::FUN_0041ace0"},
        {"0x0041bee0", "CGaming::FUN_0041bee0"},
        {"0x0041c140", "CGaming::CGaming_OnResumeOrStartGame"},
        {"0x0041d5f0", "CGaming::CGaming_OnCmd"},
        {"0x0041f050", "CGaming::CGaming_OnSchedulerTimer"},
        {"0x004206a0", "CGaming::CGaming_OnCustomEvent"},
        {"0x004216c0", "CLevelScript::FUN_004216c0"},
        {"0x00421810", "CLevelScript::FUN_00421810"},
        {"0x00421820", "CLevelScript::FUN_00421820"},
        {"0x00422770", "CLevelScript::FUN_00422770"},
        {"0x004245c0", "CDSObject::CDSObject_ReleaseViaVtable"},
        {"0x004270a0", "CNumCounter::CNumCounter_OnTimerTick"},
        {"0x004273e0", "CNumCounter::CNumCounter_GetTypeDescriptor"},
        {"0x004273f0", "CNumCounter::CNumCounter_AdjustorThunk04_Dtor"},
        {"0x00427400", "CNumCounter::CNumCounter_AdjustorThunk10_Dtor"},
        {"0x00427410", "CNumCounter::CNumCounter_AdjustorThunk18_Dtor"},
        {"0x00427420", "CNumCounter::CNumCounter_AdjustorThunk68_Dtor"},
        {"0x00427740", "CNumCounter::CNumCounter_Render"},
        {"0x00427af0", "CNumCounter::CNumCounter_vDtor"},
        {"0x00428e20", "CGame::FUN_00428e20"},
        {"0x0042c040", "CDSView::CDSView_DispatchEvent"},
        {"0x0042c0e0", "CBulanci::CDSView_OnKeyUp"},
        {"0x0042c100", "CBulanci::CDSView_OnChar"},
        {"0x0042c2e0", "CBulanci::CDSView_GetDataSize"},
        {"0x0042c320", "CBulanci::CDSView_SaveData"},
        {"0x0042c370", "CBulanci::CDSView_LoadData"},
        {"0x0042c3e0", "CBulanci::CDSView_IsModalDoneRecursive"},
        {"0x0042c430", "CBulanci::CDSView_GetParentBounds"},
        {"0x0042c480", "CBulanci::CDSView_SetRect"},
        {"0x0042c580", "CBulanci::CDSView_ComputeAnchoredRect"},
        {"0x0042c770", "CBulanci::CDSView_BroadcastEventToChildren"},
        {"0x0042ca30", "CDSView::CDSView_InvalidateRectClipped"},
        {"0x0042cae0", "CBulanci::CDSView_AdaptDisplaySize"},
        {"0x0042ccf0", "CDSApp::CDSView_RenderChildrenClipped"},
        {"0x0042cf50", "CDSView::CDSView_OnLButtonDownAcquireFocus"},
        {"0x00438340", "CBulanci::CDSView_NoOpStub"},
        {"0x00438e40", "CLevelScript::FUN_00438e40"},
        {"0x00438f80", "CBulanci::CDSView_EmptyHook27"},
        {"0x0043ca40", "CGame::FUN_0043ca40"},
    };

    @Override
    public void run() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();
        Listing listing = currentProgram.getListing();
        int vOk = 0, fOk = 0, vSkip = 0, fSkip = 0;
        for (String[] row : VFT) {
            Address a = toAddr(row[0]);
            String name = row[1];
            try {
                createLabel(a, name, true);
                vOk++;
            } catch (Exception e) {
                printf("vft skip %s %s: %s\n", row[0], name, e.getMessage());
                vSkip++;
            }
        }
        for (String[] row : FN) {
            Address a = toAddr(row[0]);
            String full = row[1];
            int sep = full.lastIndexOf("::");
            String nsName = sep > 0 ? full.substring(0, sep) : "";
            String fnName = sep > 0 ? full.substring(sep + 2) : full;
            Function f = listing.getFunctionAt(a);
            if (f == null) {
                printf("no fn %s\n", row[0]);
                fSkip++;
                continue;
            }
            try {
                if (!nsName.isEmpty()) {
                    Namespace ns = NamespaceUtils.getNamespace(nsName, currentProgram);
                    if (ns == null) {
                        ns = st.createNameSpace(null, nsName, SourceType.USER_DEFINED);
                    }
                    f.setParentNamespace(ns);
                }
                f.setName(fnName, SourceType.USER_DEFINED);
                fOk++;
            } catch (Exception e) {
                printf("fn skip %s %s: %s\n", row[0], full, e.getMessage());
                fSkip++;
            }
        }
        printf("ApplyScopedVftableRenames: vft %d ok %d skip; fn %d ok %d skip\n",
               vOk, vSkip, fOk, fSkip);
    }
}
