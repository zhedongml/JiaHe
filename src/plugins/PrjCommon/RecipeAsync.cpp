#include "RecipeAsync.h"

RecipeAsync& RecipeAsync::instance()
{
    static RecipeAsync instance;
    return instance;
}

RecipeAsync::RecipeAsync()
{
}

RecipeAsync::~RecipeAsync()
{
}

Result RecipeAsync::waitEnd()
{
    if(m_sync.futures().size() == 0){
        return Result();
    }

    m_sync.waitForFinished();

    Result ret;
    for (int i = 0; i < m_sync.futures().size(); ++i) {
        Result retTmp = m_sync.futures()[i].result();

        if (!retTmp.success) {
            if (ret.success) {
                ret.errorMsg = retTmp.errorMsg;
            }
            else {
                ret.errorMsg += "\n" + retTmp.errorMsg;
            }
            ret.success = false;
        }
    }

    m_sync.clearFutures();
    return ret;
}