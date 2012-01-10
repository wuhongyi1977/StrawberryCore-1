/*
 * Copyright (C) 2010-2012 Strawberry-Pr0jcts <http://strawberry-pr0jcts.com/>
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef STRAWBERRY_SINGLETONIMPL_H
#define STRAWBERRY_SINGLETONIMPL_H

#include "Singleton.h"

// avoid the using namespace here cuz
// its a .h file afterall

template
<
typename T,
class ThreadingModel,
class CreatePolicy,
class LifeTimePolicy
>
T& Strawberry::Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy>::Instance()
{
    if (!si_instance)
    {
        // double-checked Locking pattern
        Guard();

        if (!si_instance)
        {
            if (si_destroyed)
            {
                si_destroyed = false;
                LifeTimePolicy::OnDeadReference();
            }

            si_instance = CreatePolicy::Create();
            LifeTimePolicy::ScheduleCall(&DestroySingleton);
        }
    }

    return *si_instance;
}

template
<
typename T,
class ThreadingModel,
class CreatePolicy,
class LifeTimePolicy
>
void Strawberry::Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy>::DestroySingleton()
{
    CreatePolicy::Destroy(si_instance);
    si_instance = NULL;
    si_destroyed = true;
}

#define INSTANTIATE_SINGLETON_1(TYPE) \
    template class STRAWBERRY_DLL_DECL Strawberry::Singleton<TYPE, Strawberry::SingleThreaded<TYPE>, Strawberry::OperatorNew<TYPE>, Strawberry::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Strawberry::Singleton<TYPE, Strawberry::SingleThreaded<TYPE>, Strawberry::OperatorNew<TYPE>, Strawberry::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Strawberry::Singleton<TYPE, Strawberry::SingleThreaded<TYPE>, Strawberry::OperatorNew<TYPE>, Strawberry::ObjectLifeTime<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_2(TYPE, THREADINGMODEL) \
    template class STRAWBERRY_DLL_DECL Strawberry::Singleton<TYPE, THREADINGMODEL, Strawberry::OperatorNew<TYPE>, Strawberry::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Strawberry::Singleton<TYPE, THREADINGMODEL, Strawberry::OperatorNew<TYPE>, Strawberry::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Strawberry::Singleton<TYPE, THREADINGMODEL, Strawberry::OperatorNew<TYPE>, Strawberry::ObjectLifeTime<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_3(TYPE, THREADINGMODEL, CREATIONPOLICY ) \
    template class STRAWBERRY_DLL_DECL Strawberry::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Strawberry::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Strawberry::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Strawberry::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Strawberry::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Strawberry::ObjectLifeType<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_4(TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME) \
    template class STRAWBERRY_DLL_DECL Strawberry::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >; \
    template<> TYPE* Strawberry::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >::si_instance = 0; \
    template<> bool Strawberry::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >::si_destroyed = false

#endif
