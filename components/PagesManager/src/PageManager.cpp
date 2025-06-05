#include "PageManager.h"
#include "PageManagerLog.h"
#include <algorithm>

#define PM_EMPTY_PAGE_NAME "EMPTY_PAGE"

/*************************************
 * PAGE MANAGER BASE
 * *************************************
 */
/**
  * @brief  Page manager constructor
  * @param  factory: Pointer to the page factory
  * @retval None
  */
PageManager::PageManager(PageFactory* factory)
    : _Factory(factory)
    , _PagePrev(nullptr)
    , _PageCurrent(nullptr)
    , _RootDefaultStyle(nullptr)
{
    memset(&_AnimState, 0, sizeof(_AnimState));

    SetGlobalLoadAnimType();
}

/**
  * @brief  Page manager destructor
  * @param  None
  * @retval None
  */
PageManager::~PageManager()
{
    SetStackClear();
}

/**
  * @brief  Search pages in the page pool
  * @param  name: Page name
  * @retval A pointer to the base class of the page, or nullptr if not found
  */
PageBase* PageManager::FindPageInPool(const char* name)
{
    for (auto iter : _PagePool)
    {
        if (strcmp(name, iter->_Name) == 0)
        {
            return iter;
        }
    }
    return nullptr;
}

/**
  * @brief  Search pages in the page stack
  * @param  name: Page name
  * @retval A pointer to the base class of the page, or nullptr if not found
  */
PageBase* PageManager::FindPageInStack(const char* name)
{
    decltype(_PageStack) stk = _PageStack;
    while (!stk.empty())
    {
        PageBase* base = stk.top();

        if (strcmp(name, base->_Name) == 0)
        {
            return base;
        }

        stk.pop();
    }

    return nullptr;
}

/**
  * @brief  Install the page, and register the page to the page pool
  * @param  className: The class name of the page
  * @param  appName: Page application name, no duplicates allowed
  * @retval Return true if successful
  */
bool PageManager::Install(const char* className, const char* appName)
{
    if (_Factory == nullptr)
    {
        PM_LOG_ERROR("Factory was not registered, can't install page");
        return false;
    }

    if (appName == nullptr)
    {
        PM_LOG_WARN("appName has not set");
        appName = className;
    }

    if (FindPageInPool(appName) != nullptr)
    {
        PM_LOG_ERROR("Page(%s) was registered", appName);
        return false;
    }

    PageBase* base = _Factory->CreatePage(className);
    if (base == nullptr)
    {
        PM_LOG_ERROR("Factory has not %s", className);
        return false;
    }

    base->_root = nullptr;
    base->_ID = 0;
    base->_Manager = nullptr;
    base->_UserData = nullptr;
    memset(&base->priv, 0, sizeof(base->priv));

    PM_LOG_INFO("Install Page[class = %s, name = %s]", className, appName);
    bool retval = Register(base, appName);

    base->onCustomAttrConfig();

    return retval;
}

/**
  * @brief  Uninstall page
  * @param  appName: Page application name, no duplicates allowed
  * @retval Return true if the uninstallation is successful
  */
bool PageManager::Uninstall(const char* appName)
{
    PM_LOG_INFO("Page(%s) uninstall...", appName);

    PageBase* base = FindPageInPool(appName);
    if (base == nullptr)
    {
        PM_LOG_ERROR("Page(%s) was not found", appName);
        return false;
    }

    if (!Unregister(appName))
    {
        PM_LOG_ERROR("Page(%s) unregister failed", appName);
        return false;
    }

    if (base->priv.IsCached)
    {
        PM_LOG_WARN("Page(%s) has cached, unloading...", appName);
        base->priv.State = PageBase::PAGE_STATE_UNLOAD;
        StateUpdate(base);
    }
    else
    {
        PM_LOG_INFO("Page(%s) has not cache", appName);
    }

    delete base;
    PM_LOG_INFO("Uninstall OK");
    return true;
}

/**
  * @brief  Register the page to the page pool
  * @param  name: Page application name, duplicate registration is not allowed
  * @retval Return true if the registration is successful
  */
bool PageManager::Register(PageBase* base, const char* name)
{
    if (FindPageInPool(name) != nullptr)
    {
        PM_LOG_ERROR("Page(%s) was multi registered", name);
        return false;
    }

    base->_Manager = this;
    base->_Name = name;

    _PagePool.push_back(base);

    return true;
}

/**
  * @brief  Log out the page from the page pool
  * @param  name: Page application name
  * @retval Return true if the logout is successful
  */
bool PageManager::Unregister(const char* name)
{
    PM_LOG_INFO("Page(%s) unregister...", name);

    PageBase* base = FindPageInStack(name);

    if (base != nullptr)
    {
        PM_LOG_ERROR("Page(%s) was in stack", name);
        return false;
    }

    base = FindPageInPool(name);
    if (base == nullptr)
    {
        PM_LOG_ERROR("Page(%s) was not found", name);
        return false;
    }

    auto iter = std::find(_PagePool.begin(), _PagePool.end(), base);

    if (iter == _PagePool.end())
    {
        PM_LOG_ERROR("Page(%s) was not found in PagePool", name);
        return false;
    }

    _PagePool.erase(iter);

    PM_LOG_INFO("Unregister OK");
    return true;
}

/**
  * @brief  Get the top page of the page stack
  * @param  None
  * @retval A pointer to the base class of the page
  */
PageBase* PageManager::GetStackTop()
{
    return _PageStack.empty() ? nullptr : _PageStack.top();
}

/**
  * @brief  Get the page below the top of the page stack
  * @param  None
  * @retval A pointer to the base class of the page
  */
PageBase* PageManager::GetStackTopAfter()
{
    PageBase* top = GetStackTop();

    if (top == nullptr)
    {
        return nullptr;
    }

    _PageStack.pop();

    PageBase* topAfter = GetStackTop();

    _PageStack.push(top);

    return topAfter;
}

/**
  * @brief  Clear the page stack and end the life cycle of all pages in the page stack
  * @param  keepBottom: Whether to keep the bottom page of the stack
  * @retval None
  */
void PageManager::SetStackClear(bool keepBottom)
{
    while (1)
    {
        PageBase* top = GetStackTop();

        if (top == nullptr)
        {
            PM_LOG_INFO("Page stack is empty, breaking...");
            break;
        }

        PageBase* topAfter = GetStackTopAfter();

        if (topAfter == nullptr)
        {
            if (keepBottom)
            {
                _PagePrev = top;
                PM_LOG_INFO("Keep page stack bottom(%s), breaking...", top->_Name);
                break;
            }
            else
            {
                _PagePrev = nullptr;
            }
        }

        FourceUnload(top);

        _PageStack.pop();
    }
    PM_LOG_INFO("Stack clear done");
}

/**
  * @brief  Get the name of the previous page
  * @param  None
  * @retval The name of the previous page, if it does not exist, return PM_EMPTY_PAGE_NAME
  */
const char* PageManager::GetPagePrevName()
{
    return _PagePrev ? _PagePrev->_Name : PM_EMPTY_PAGE_NAME;
}

/**********************************
 * PAGE MANAGER ANIMATION
 * *************************************
 */
/**
  * @brief  Get page loading animation properties
  * @param  anim: Animation type
  * @param  attr: Pointer to attribute
  * @retval Whether the acquisition is successful
  */
bool PageManager::GetLoadAnimAttr(uint8_t anim, LoadAnimAttr_t* attr)
{
    lv_coord_t hor = LV_HOR_RES;
    lv_coord_t ver = LV_VER_RES;

    switch (anim)
    {
    case LOAD_ANIM_OVER_LEFT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = hor;
        break;

    case LOAD_ANIM_OVER_RIGHT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = -hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -hor;
        break;

    case LOAD_ANIM_OVER_TOP:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = ver;
        break;

    case LOAD_ANIM_OVER_BOTTOM:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = -ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -ver;
        break;

    case LOAD_ANIM_MOVE_LEFT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = -hor;

        attr->pop.enter.start = -hor;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = hor;
        break;

    case LOAD_ANIM_MOVE_RIGHT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = -hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = hor;

        attr->pop.enter.start = hor;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -hor;
        break;

    case LOAD_ANIM_MOVE_TOP:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = -ver;

        attr->pop.enter.start = -ver;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = ver;
        break;

    case LOAD_ANIM_MOVE_BOTTOM:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = -ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = ver;

        attr->pop.enter.start = ver;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -ver;
        break;

    case LOAD_ANIM_FADE_ON:
        attr->dragDir = ROOT_DRAG_DIR_NONE;

        attr->push.enter.start = LV_OPA_TRANSP;
        attr->push.enter.end = LV_OPA_COVER;
        attr->push.exit.start = LV_OPA_COVER;
        attr->push.exit.end = LV_OPA_COVER;

        attr->pop.enter.start = LV_OPA_COVER;
        attr->pop.enter.end = LV_OPA_COVER;
        attr->pop.exit.start = LV_OPA_COVER;
        attr->pop.exit.end = LV_OPA_TRANSP;
        break;

    case LOAD_ANIM_NONE:
        memset(attr, 0, sizeof(LoadAnimAttr_t));
        return true;

    default:
        PM_LOG_ERROR("Load anim type error: %d", anim);
        return false;
    }

    /* Determine the setter and getter of the animation */
    if (attr->dragDir == ROOT_DRAG_DIR_HOR)
    {
        attr->setter = [](void* obj, int32_t v)
        {
            lv_obj_set_x((lv_obj_t*)obj, v);
        };
        attr->getter = [](void* obj)
        {
            return (int32_t)lv_obj_get_x((lv_obj_t*)obj);
        };
    }
    else if (attr->dragDir == ROOT_DRAG_DIR_VER)
    {
        attr->setter = [](void* obj, int32_t v)
        {
            lv_obj_set_y((lv_obj_t*)obj, v);
        };
        attr->getter = [](void* obj)
        {
            return (int32_t)lv_obj_get_y((lv_obj_t*)obj);
        };
    }
    else
    {
        attr->setter = [](void* obj, int32_t v)
        {
            lv_obj_set_style_bg_opa((lv_obj_t*)obj, (lv_opa_t)v, LV_PART_MAIN);
        };
        attr->getter = [](void* obj)
        {
            return (int32_t)lv_obj_get_style_bg_opa((lv_obj_t*)obj, LV_PART_MAIN);
        };
    }

    return true;
}

/**********************************
 * PAGE MANAGER DRAG
 * *************************************
 */
#define CONSTRAIN(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

/* The distance threshold to trigger the drag */
#define PM_INDEV_DEF_DRAG_THROW    20

/**
  * @brief  Page drag event callback
  * @param  event: Pointer to event structure
  * @retval None
  */
void PageManager::onRootDragEvent(lv_event_t* event)
{
    lv_event_code_t eventCode = lv_event_get_code(event);

    if (!(eventCode == LV_EVENT_PRESSED || eventCode == LV_EVENT_PRESSING || eventCode == LV_EVENT_RELEASED))
    {
        return;
    }

    lv_obj_t* root = lv_event_get_current_target(event);
    PageBase* base = (PageBase*)lv_event_get_user_data(event);

    if (base == nullptr)
    {
        PM_LOG_ERROR("Page base is NULL");
        return;
    }

    PageManager* manager = base->_Manager;
    LoadAnimAttr_t animAttr;

    if (!manager->GetCurrentLoadAnimAttr(&animAttr))
    {
        PM_LOG_ERROR("Can't get current anim attr");
        return;
    }

    if (eventCode == LV_EVENT_PRESSED)
    {
        if (manager->_AnimState.IsSwitchReq)
        {
            return;
        }

        if (!manager->_AnimState.IsBusy)
        {
            return;
        }  

        PM_LOG_INFO("Root anim interrupted");
        lv_anim_del(root, animAttr.setter);
        manager->_AnimState.IsBusy = false;

        /* Temporary showing the bottom page */
        PageBase* bottomPage = manager->GetStackTopAfter();
        lv_obj_clear_flag(bottomPage->_root, LV_OBJ_FLAG_HIDDEN);
    }
    else if (eventCode == LV_EVENT_PRESSING)
    {
        lv_coord_t cur = animAttr.getter(root);

        lv_coord_t max = std::max(animAttr.pop.exit.start, animAttr.pop.exit.end);
        lv_coord_t min = std::min(animAttr.pop.exit.start, animAttr.pop.exit.end);

        lv_point_t offset;
        lv_indev_get_vect(lv_indev_get_act(), &offset);

        if (animAttr.dragDir == ROOT_DRAG_DIR_HOR)
        {
            cur += offset.x;
        }
        else if (animAttr.dragDir == ROOT_DRAG_DIR_VER)
        {
            cur += offset.y;
        }

        animAttr.setter(root, CONSTRAIN(cur, min, max));
    }
    else if (eventCode == LV_EVENT_RELEASED)
    {
        if (manager->_AnimState.IsSwitchReq)
        {
            return;
        }

        lv_coord_t offset_sum = animAttr.push.enter.end - animAttr.push.enter.start;

        lv_coord_t x_predict = 0;
        lv_coord_t y_predict = 0;
        RootGetDragPredict(&x_predict, &y_predict);

        lv_coord_t start = animAttr.getter(root);
        lv_coord_t end = start;

        if (animAttr.dragDir == ROOT_DRAG_DIR_HOR)
        {
            end += x_predict;
            PM_LOG_INFO("Root drag x_predict = %d", end);
        }
        else if (animAttr.dragDir == ROOT_DRAG_DIR_VER)
        {
            end += y_predict;
            PM_LOG_INFO("Root drag y_predict = %d", end);
        }

        if (std::abs(end) > std::abs((int)offset_sum) / 2)
        {
            lv_async_call(onRootAsyncLeave, base);
        }
        else if(end != animAttr.push.enter.end)
        {
            manager->_AnimState.IsBusy = true;

            lv_anim_t a;
            manager->AnimDefaultInit(&a);
            lv_anim_set_user_data(&a, manager);
            lv_anim_set_var(&a, root);
            lv_anim_set_values(&a, start, animAttr.push.enter.end);
            lv_anim_set_exec_cb(&a, animAttr.setter);
            lv_anim_set_ready_cb(&a, onRootDragAnimFinish);
            lv_anim_start(&a);
            PM_LOG_INFO("Root drag anim start");
        }
    }
}

/**
  * @brief  Drag animation end event callback
  * @param  a: Pointer to animation
  * @retval None
  */
void PageManager::onRootDragAnimFinish(lv_anim_t* a)
{
    PageManager* manager = (PageManager*)lv_anim_get_user_data(a);
    PM_LOG_INFO("Root drag anim finish");
    manager->_AnimState.IsBusy = false;

    /* Hide the bottom page */
    PageBase* bottomPage = manager->GetStackTopAfter();
    if (bottomPage)
    {
        lv_obj_add_flag(bottomPage->_root, LV_OBJ_FLAG_HIDDEN);
    }
}

/**
  * @brief  Enable root's drag function
  * @param  root: Pointer to the root object
  * @retval None
  */
void PageManager::RootEnableDrag(lv_obj_t* root)
{
    PageBase* base = (PageBase*)lv_obj_get_user_data(root);
    lv_obj_add_event_cb(
        root,
        onRootDragEvent,
        LV_EVENT_ALL,
        base
    );
    PM_LOG_INFO("Page(%s) Root drag enabled", base->_Name);
}

/**
  * @brief  Asynchronous callback when dragging ends
  * @param  data: Pointer to the base class of the page
  * @retval None
  */
void PageManager::onRootAsyncLeave(void* data)
{
    PageBase* base = (PageBase*)data;
    PM_LOG_INFO("Page(%s) send event: LV_EVENT_LEAVE, need to handle...", base->_Name);
    lv_event_send(base->_root, LV_EVENT_LEAVE, base);
}

/**
  * @brief  Get drag inertia prediction stop point
  * @param  x: x stop point
  * @param  y: y stop point
  * @retval None
  */
void PageManager::RootGetDragPredict(lv_coord_t* x, lv_coord_t* y)
{
    lv_indev_t* indev = lv_indev_get_act();
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);

    lv_coord_t y_predict = 0;
    lv_coord_t x_predict = 0;

    while (vect.y != 0)
    {
        y_predict += vect.y;
        vect.y = vect.y * (100 - PM_INDEV_DEF_DRAG_THROW) / 100;
    }

    while (vect.x != 0)
    {
        x_predict += vect.x;
        vect.x = vect.x * (100 - PM_INDEV_DEF_DRAG_THROW) / 100;
    }

    *x = x_predict;
    *y = y_predict;
}

/**********************************
 * PAGE MANAGER ROUTER
 * *************************************
 */
/**
   * @brief  Enter a new page, replace the old page
   * @param  name: The name of the page to enter
   * @param  stash: Parameters passed to the new page
   * @retval Return true if successful
   */
bool PageManager::Replace(const char* name, const PageBase::Stash_t* stash)
{
    /* Check whether the animation of switching pages is being executed */
    if (!SwitchAnimStateCheck())
    {
        return false;
    }

    /* Check whether the stack is repeatedly pushed  */
    if (FindPageInStack(name) != nullptr)
    {
        PM_LOG_ERROR("Page(%s) was multi push", name);
        return false;
    }

    /* Check if the page is registered in the page pool */
    PageBase* base = FindPageInPool(name);

    if (base == nullptr)
    {
        PM_LOG_ERROR("Page(%s) was not install", name);
        return false;
    }

    /* Get the top page of the stack */
    PageBase* top = GetStackTop();

    if (top == nullptr)
    {
        PM_LOG_ERROR("Stack top is NULL");
        return false;
    }

    /* Force disable cache */
    top->priv.IsCached = false;

    /* Synchronous automatic cache configuration */
    base->priv.IsDisableAutoCache = base->priv.ReqDisableAutoCache;

    /* Remove current page */
    _PageStack.pop();

    /* Push into the stack */
    _PageStack.push(base);

    PM_LOG_INFO("Page(%s) replace Page(%s) (stash = 0x%p)", name, top->_Name, stash);

    /* Page switching execution */
    return SwitchTo(base, true, stash);
}

/**
  * @brief  Enter a new page, the old page is pushed onto the stack
  * @param  name: The name of the page to enter 
  * @param  stash: Parameters passed to the new page
  * @retval Return true if successful
  */
bool PageManager::Push(const char* name, const PageBase::Stash_t* stash)
{
    /* Check whether the animation of switching pages is being executed */
    if (!SwitchAnimStateCheck())
    {
        return false;
    }

    /* Check whether the stack is repeatedly pushed  */
    if (FindPageInStack(name) != nullptr)
    {
        PM_LOG_ERROR("Page(%s) was multi push", name);
        return false;
    }

    /* Check if the page is registered in the page pool */
    PageBase* base = FindPageInPool(name);

    if (base == nullptr)
    {
        PM_LOG_ERROR("Page(%s) was not install", name);
        return false;
    }

    /* Synchronous automatic cache configuration */
    base->priv.IsDisableAutoCache = base->priv.ReqDisableAutoCache;

    /* Push into the stack */
    _PageStack.push(base);

    PM_LOG_INFO("Page(%s) push >> [Screen] (stash = 0x%p)", name, stash);

    /* Page switching execution */
    return SwitchTo(base, true, stash);
}

/**
  * @brief  Pop the current page
  * @param  None
  * @retval Return true if successful
  */
bool PageManager::Pop()
{
    /* Check whether the animation of switching pages is being executed */
    if (!SwitchAnimStateCheck())
    {
        return false;
    }

    /* Get the top page of the stack */
    PageBase* top = GetStackTop();

    if (top == nullptr)
    {
        PM_LOG_WARN("Page stack is empty, cat't pop");
        return false;
    }

    /* Whether to turn off automatic cache */
    if (!top->priv.IsDisableAutoCache)
    {
        PM_LOG_INFO("Page(%s) has auto cache, cache disabled", top->_Name);
        top->priv.IsCached = false;
    }

    PM_LOG_INFO("Page(%s) pop << [Screen]", top->_Name);

    /* Page popup */
    _PageStack.pop();

    /* Get the next page */
    top = GetStackTop();

    /* Page switching execution */
    return SwitchTo(top, false, nullptr);;
}

/**
  * @brief  Page switching
  * @param  newNode: Pointer to new page
  * @param  isEnterAct: Whether it is a ENTER action
  * @param  stash: Parameters passed to the new page
  * @retval Return true if successful
  */
bool PageManager::SwitchTo(PageBase* newNode, bool isEnterAct, const PageBase::Stash_t* stash)
{
    if (newNode == nullptr)
    {
        PM_LOG_ERROR("newNode is nullptr");
        return false;
    }

    /* Whether page switching has been requested */
    if (_AnimState.IsSwitchReq)
    {
        PM_LOG_WARN("Page switch busy, reqire(%s) is ignore", newNode->_Name);
        return false;
    }

    _AnimState.IsSwitchReq = true;

    /* Is there a parameter to pass */
    if (stash != nullptr)
    {
        PM_LOG_INFO("stash is detect, %s >> stash(0x%p) >> %s", GetPagePrevName(), stash, newNode->_Name);

        void* buffer = nullptr;

        if (newNode->priv.Stash.ptr == nullptr)
        {
            buffer = lv_mem_alloc(stash->size);
            if (buffer == nullptr)
            {
                PM_LOG_ERROR("stash malloc failed");
            }
            else
            {
                PM_LOG_INFO("stash(0x%p) malloc[%ld]", buffer, stash->size);
            }
        }
        else if(newNode->priv.Stash.size == stash->size)
        {
            buffer = newNode->priv.Stash.ptr;
            PM_LOG_INFO("stash(0x%p) is exist", buffer);
        }

        if (buffer != nullptr)
        {
            memcpy(buffer, stash->ptr, stash->size);
            PM_LOG_INFO("stash memcpy[%ld] 0x%p >> 0x%p", stash->size, stash->ptr, buffer);
            newNode->priv.Stash.ptr = buffer;
            newNode->priv.Stash.size = stash->size;
        }
    }

    /* Record current page */
    _PageCurrent = newNode;

    /* If the current page has a cache */
    if (_PageCurrent->priv.IsCached)
    {
        /* Direct display, no need to load */
        PM_LOG_INFO("Page(%s) has cached, appear driectly", _PageCurrent->_Name);
        _PageCurrent->priv.State = PageBase::PAGE_STATE_WILL_APPEAR;
    }
    else
    {
        /* Load page */
        _PageCurrent->priv.State = PageBase::PAGE_STATE_LOAD;
    }

    if (_PagePrev != nullptr)
    {
        _PagePrev->priv.Anim.IsEnter = false;
    }

    _PageCurrent->priv.Anim.IsEnter = true;

    _AnimState.IsEntering = isEnterAct;

    if (_AnimState.IsEntering)
    {
        /* Update the animation configuration according to the current page */
        SwitchAnimTypeUpdate(_PageCurrent);
    }

    /* Update the state machine of the previous page */
    StateUpdate(_PagePrev);

    /* Update the state machine of the current page */
    StateUpdate(_PageCurrent);

    /* Move the layer, move the new page to the front */
    if (_AnimState.IsEntering)
    {
        PM_LOG_INFO("Page ENTER is detect, move Page(%s) to foreground", _PageCurrent->_Name);
        if (_PagePrev)lv_obj_move_foreground(_PagePrev->_root);
        lv_obj_move_foreground(_PageCurrent->_root);
    }
    else
    {
        PM_LOG_INFO("Page EXIT is detect, move Page(%s) to foreground", GetPagePrevName());
        lv_obj_move_foreground(_PageCurrent->_root);
        if (_PagePrev)lv_obj_move_foreground(_PagePrev->_root);
    }
    return true;
}

/**
  * @brief  Force the end of the life cycle of the page without animation 
  * @param  base: Pointer to the page being executed
  * @retval Return true if successful
  */
bool PageManager::FourceUnload(PageBase* base)
{
    if (base == nullptr)
    {
        PM_LOG_ERROR("Page is nullptr, Unload failed");
        return false;
    }

    PM_LOG_INFO("Page(%s) Fource unloading...", base->_Name);

    if (base->priv.State == PageBase::PAGE_STATE_ACTIVITY)
    {
        PM_LOG_INFO("Page state is ACTIVITY, Disappearing...");
        base->onViewWillDisappear();
        base->onViewDidDisappear();
    }

    base->priv.State = StateUnloadExecute(base);

    return true;
}

/**
  * @brief  Back to the main page (the page at the bottom of the stack) 
  * @param  None
  * @retval Return true if successful
  */
bool PageManager::BackHome()
{
    /* Check whether the animation of switching pages is being executed */
    if (!SwitchAnimStateCheck())
    {
        return false;
    }

    SetStackClear(true);

    _PagePrev = nullptr;

    PageBase* home = GetStackTop();

    SwitchTo(home, false);

    return true;
}

/**
  * @brief  Check if the page switching animation is being executed
  * @param  None
  * @retval Return true if it is executing
  */
bool PageManager::SwitchAnimStateCheck()
{
    if (_AnimState.IsSwitchReq || _AnimState.IsBusy)
    {
        PM_LOG_WARN(
            "Page switch busy[AnimState.IsSwitchReq = %d,"
            "AnimState.IsBusy = %d],"
            "request ignored",
            _AnimState.IsSwitchReq,
            _AnimState.IsBusy
        );
        return false;
    }

    return true;
}

/**
  * @brief  Page switching request check 
  * @param  None
  * @retval Return true if all pages are executed
  */
bool PageManager::SwitchReqCheck()
{
    bool ret = false;
    bool lastNodeBusy = _PagePrev && _PagePrev->priv.Anim.IsBusy;

    if (!_PageCurrent->priv.Anim.IsBusy && !lastNodeBusy)
    {
        PM_LOG_INFO("----Page switch was all finished----");
        _AnimState.IsSwitchReq = false;
        ret = true;
        _PagePrev = _PageCurrent;
    }
    else
    {
        if (_PageCurrent->priv.Anim.IsBusy)
        {
            PM_LOG_WARN("Page PageCurrent(%s) is busy", _PageCurrent->_Name);
        }
        else
        {
            PM_LOG_WARN("Page PagePrev(%s) is busy", GetPagePrevName());
        }
    }

    return ret;
}

/**
  * @brief  PPage switching animation execution end callback 
  * @param  a: Pointer to animation
  * @retval None
  */
void PageManager::onSwitchAnimFinish(lv_anim_t* a)
{
    PageBase* base = (PageBase*)lv_anim_get_user_data(a);
    PageManager* manager = base->_Manager;

    PM_LOG_INFO("Page(%s) Anim finish", base->_Name);

    manager->StateUpdate(base);
    base->priv.Anim.IsBusy = false;
    bool isFinished = manager->SwitchReqCheck();

    if (!manager->_AnimState.IsEntering && isFinished)
    {
        manager->SwitchAnimTypeUpdate(manager->_PageCurrent);
    }
}

/**
  * @brief  Create page switching animation
  * @param  a: Point to the animated page
  * @retval None
  */
void PageManager::SwitchAnimCreate(PageBase* base)
{
    LoadAnimAttr_t animAttr;
    if (!GetCurrentLoadAnimAttr(&animAttr))
    {
        return;
    }

    lv_anim_t a;
    AnimDefaultInit(&a);
    lv_anim_set_user_data(&a, base);
    lv_anim_set_var(&a, base->_root);
    lv_anim_set_ready_cb(&a, onSwitchAnimFinish);
    lv_anim_set_exec_cb(&a, animAttr.setter);

    int32_t start = 0;

    if (animAttr.getter)
    {
        start = animAttr.getter(base->_root);
    }

    if (_AnimState.IsEntering)
    {
        if (base->priv.Anim.IsEnter)
        {
            lv_anim_set_values(
                &a,
                animAttr.push.enter.start,
                animAttr.push.enter.end
            );
        }
        else /* Exit */
        {
            lv_anim_set_values(
                &a,
                start,
                animAttr.push.exit.end
            );
        }
    }
    else /* Pop */
    {
        if (base->priv.Anim.IsEnter)
        {
            lv_anim_set_values(
                &a,
                animAttr.pop.enter.start,
                animAttr.pop.enter.end
            );
        }
        else /* Exit */
        {
            lv_anim_set_values(
                &a,
                start,
                animAttr.pop.exit.end
            );
        }
    }

    lv_anim_start(&a);
    base->priv.Anim.IsBusy = true;
}

/**
  * @brief  Set global animation properties 
  * @param  anim: Animation type
  * @param  time: Animation duration
  * @param  path: Animation curve
  * @retval None
  */
void PageManager::SetGlobalLoadAnimType(LoadAnim_t anim, uint16_t time, lv_anim_path_cb_t path)
{
    if (anim > _LOAD_ANIM_LAST)
    {
        anim = LOAD_ANIM_NONE;
    }

    _AnimState.Global.Type = anim;
    _AnimState.Global.Time = time;
    _AnimState.Global.Path = path;

    PM_LOG_INFO("Set global load anim type = %d", anim);
}

/**
  * @brief  Update current animation properties, apply page custom animation
  * @param  base: Pointer to page
  * @retval None
  */
void PageManager::SwitchAnimTypeUpdate(PageBase* base)
{
    if (base->priv.Anim.Attr.Type == LOAD_ANIM_GLOBAL)
    {
        PM_LOG_INFO(
            "Page(%s) Anim.Type was not set, use AnimState.Global.Type = %d",
            base->_Name,
            _AnimState.Global.Type
        );
        _AnimState.Current = _AnimState.Global;
    }
    else
    {
        if (base->priv.Anim.Attr.Type > _LOAD_ANIM_LAST)
        {
            PM_LOG_ERROR(
                "Page(%s) ERROR custom Anim.Type = %d, use AnimState.Global.Type = %d",
                base->_Name,
                base->priv.Anim.Attr.Type,
                _AnimState.Global.Type
            );
            base->priv.Anim.Attr = _AnimState.Global;
        }
        else
        {
            PM_LOG_INFO(
                "Page(%s) custom Anim.Type set = %d",
                base->_Name,
                base->priv.Anim.Attr.Type
            );
        }
        _AnimState.Current = base->priv.Anim.Attr;
    }
}

/**
  * @brief  Set animation default parameters
  * @param  a: Pointer to animation
  * @retval None
  */
void PageManager::AnimDefaultInit(lv_anim_t* a)
{
    lv_anim_init(a);

    uint32_t time = (GetCurrentLoadAnimType() == LOAD_ANIM_NONE) ? 0 : _AnimState.Current.Time;
    lv_anim_set_time(a, time);
    lv_anim_set_path_cb(a, _AnimState.Current.Path);
}
/**********************************
 * PAGE MANAGER STATE
 * *************************************
 */
/**
  * @brief  Update page state machine
  * @param  base: Pointer to the updated page
  * @retval None
  */
void PageManager::StateUpdate(PageBase* base)
{
    if (base == nullptr)
        return;

    switch (base->priv.State)
    {
    case PageBase::PAGE_STATE_IDLE:
        PM_LOG_INFO("Page(%s) state idle", base->_Name);
        break;

    case PageBase::PAGE_STATE_LOAD:
        base->priv.State = StateLoadExecute(base);
        StateUpdate(base);
        break;

    case PageBase::PAGE_STATE_WILL_APPEAR:
        base->priv.State = StateWillAppearExecute(base);
        break;

    case PageBase::PAGE_STATE_DID_APPEAR:
        base->priv.State = StateDidAppearExecute(base);
        PM_LOG_INFO("Page(%s) state active", base->_Name);
        break;

    case PageBase::PAGE_STATE_ACTIVITY:
        PM_LOG_INFO("Page(%s) state active break", base->_Name);
        base->priv.State = PageBase::PAGE_STATE_WILL_DISAPPEAR;
        StateUpdate(base);
        break;

    case PageBase::PAGE_STATE_WILL_DISAPPEAR:
        base->priv.State = StateWillDisappearExecute(base);
        break;

    case PageBase::PAGE_STATE_DID_DISAPPEAR:
        base->priv.State = StateDidDisappearExecute(base);
        if (base->priv.State == PageBase::PAGE_STATE_UNLOAD)
        {
            StateUpdate(base);
        }
        break;

    case PageBase::PAGE_STATE_UNLOAD:
        base->priv.State = StateUnloadExecute(base);
        break;

    default:
        PM_LOG_ERROR("Page(%s) state[%d] was NOT FOUND!", base->_Name, base->priv.State);
        break;
    }
}

/**
  * @brief  Page loading status
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateLoadExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state load", base->_Name);

    if (base->_root != nullptr)
    {
        PM_LOG_ERROR("Page(%s) root must be nullptr", base->_Name);
    }

    lv_obj_t* root_obj = lv_obj_create(lv_scr_act());
    
    lv_obj_clear_flag(root_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_user_data(root_obj, base);

    if (_RootDefaultStyle)
    {
        lv_obj_add_style(root_obj, _RootDefaultStyle, LV_PART_MAIN);
    }

    base->_root = root_obj;
    base->onViewLoad();

    if (GetIsOverAnim(GetCurrentLoadAnimType()))
    {
        PageBase* bottomPage = GetStackTopAfter();

        if (bottomPage != nullptr && bottomPage->priv.IsCached)
        {
            LoadAnimAttr_t animAttr;
            if (GetCurrentLoadAnimAttr(&animAttr))
            {
                if (animAttr.dragDir != ROOT_DRAG_DIR_NONE)
                {
                    RootEnableDrag(base->_root);
                }
            }
        }
    }

    base->onViewDidLoad();

    if (base->priv.IsDisableAutoCache)
    {
        PM_LOG_INFO("Page(%s) disable auto cache, ReqEnableCache = %d", base->_Name, base->priv.ReqEnableCache);
        base->priv.IsCached = base->priv.ReqEnableCache;
    }
    else
    {
        PM_LOG_INFO("Page(%s) AUTO cached", base->_Name);
        base->priv.IsCached = true;
    }

    return PageBase::PAGE_STATE_WILL_APPEAR;
}

/**
  * @brief  The page is about to show the status
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateWillAppearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state will appear", base->_Name);
    base->onViewWillAppear();
    lv_obj_clear_flag(base->_root, LV_OBJ_FLAG_HIDDEN);
    SwitchAnimCreate(base);
    return PageBase::PAGE_STATE_DID_APPEAR;
}

/**
  * @brief  The status of the page display
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateDidAppearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state did appear", base->_Name);
    base->onViewDidAppear();
    return PageBase::PAGE_STATE_ACTIVITY;
}

/**
  * @brief  The page is about to disappear
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateWillDisappearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state will disappear", base->_Name);
    base->onViewWillDisappear();
    SwitchAnimCreate(base);
    return PageBase::PAGE_STATE_DID_DISAPPEAR;
}

/**
  * @brief  Page disappeared end state
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateDidDisappearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state did disappear", base->_Name);
    lv_obj_add_flag(base->_root, LV_OBJ_FLAG_HIDDEN);
    base->onViewDidDisappear();
    if (base->priv.IsCached)
    {
        PM_LOG_INFO("Page(%s) has cached", base->_Name);
        return PageBase::PAGE_STATE_WILL_APPEAR;
    }
    else
    {
        return PageBase::PAGE_STATE_UNLOAD;
    }
}

/**
  * @brief  Page unload complete
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateUnloadExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state unload", base->_Name);
    if (base->_root == nullptr)
    {
        PM_LOG_WARN("Page is loaded!");
        goto Exit;
    }

    base->onViewUnload();
    if (base->priv.Stash.ptr != nullptr && base->priv.Stash.size != 0)
    {
        PM_LOG_INFO("Page(%s) free stash(0x%p)[%ld]", base->_Name, base->priv.Stash.ptr, base->priv.Stash.size);
        lv_mem_free(base->priv.Stash.ptr);
        base->priv.Stash.ptr = nullptr;
        base->priv.Stash.size = 0;
    }

    /* Delete after the end of the root animation life cycle */
    lv_obj_del_async(base->_root);
    base->_root = nullptr;
    base->priv.IsCached = false;
    base->onViewDidUnload();

Exit:
    return PageBase::PAGE_STATE_IDLE;
}
