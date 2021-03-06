//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_MANAGED_MAPPED_FILE_HPP
#define BOOST_INTERPROCESS_MANAGED_MAPPED_FILE_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/detail/managed_open_or_create_impl.hpp>
#include <boost/interprocess/detail/managed_memory_impl.hpp>
#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/file_wrapper.hpp>
#include <boost/interprocess/detail/move.hpp>
#include <boost/interprocess/file_mapping.hpp>

namespace boost {
namespace interprocess {

//!A basic mapped file named object creation class. Initializes the 
//!mapped file. Inherits all basic functionality from 
//!basic_managed_memory_impl<CharType, AllocationAlgorithm, IndexType>
template
      <
         class CharType, 
         class AllocationAlgorithm, 
         template<class IndexConfig> class IndexType
      >
class basic_managed_mapped_file 
   : public detail::basic_managed_memory_impl
      <CharType, AllocationAlgorithm, IndexType
      ,detail::managed_open_or_create_impl<detail::file_wrapper>::ManagedOpenOrCreateUserOffset>
{
   /// @cond
   public:
   typedef detail::basic_managed_memory_impl 
      <CharType, AllocationAlgorithm, IndexType,
      detail::managed_open_or_create_impl<detail::file_wrapper>::ManagedOpenOrCreateUserOffset>   base_t;
   typedef detail::file_wrapper device_type;

   private:

   typedef detail::create_open_func<base_t>        create_open_func_t;   
   typedef detail::managed_open_or_create_impl<detail::file_wrapper> managed_open_or_create_type;

   basic_managed_mapped_file *get_this_pointer()
   {  return this;   }

   private:
   typedef typename base_t::char_ptr_holder_t   char_ptr_holder_t;
   BOOST_INTERPROCESS_MOVABLE_BUT_NOT_COPYABLE(basic_managed_mapped_file)
   /// @endcond

   public: //functions

   //!Creates mapped file and creates and places the segment manager. 
   //!This can throw.
   basic_managed_mapped_file()
   {}

   //!Creates mapped file and creates and places the segment manager. 
   //!This can throw.
   basic_managed_mapped_file(create_only_t create_only, const char *name,
                             std::size_t size, const void *addr = 0)
      : m_mfile(create_only, name, size, read_write, addr, 
                create_open_func_t(get_this_pointer(), detail::DoCreate))
   {}

   //!Creates mapped file and creates and places the segment manager if
   //!segment was not created. If segment was created it connects to the
   //!segment.
   //!This can throw.
   basic_managed_mapped_file (open_or_create_t open_or_create,
                              const char *name, std::size_t size, 
                              const void *addr = 0)
      : m_mfile(open_or_create, name, size, read_write, addr, 
                create_open_func_t(get_this_pointer(), 
                detail::DoOpenOrCreate))
   {}

   //!Connects to a created mapped file and its segment manager.
   //!This can throw.
   basic_managed_mapped_file (open_only_t open_only, const char* name, 
                              const void *addr = 0)
      : m_mfile(open_only, name, read_write, addr, 
                create_open_func_t(get_this_pointer(), 
                detail::DoOpen))
   {}

   //!Connects to a created mapped file and its segment manager
   //!in copy_on_write mode.
   //!This can throw.
   basic_managed_mapped_file (open_copy_on_write_t, const char* name, 
                              const void *addr = 0)
      : m_mfile(open_only, name, copy_on_write, addr, 
                create_open_func_t(get_this_pointer(), 
                detail::DoOpen))
   {}

   //!Connects to a created mapped file and its segment manager
   //!in read-only mode.
   //!This can throw.
   basic_managed_mapped_file (open_read_only_t, const char* name, 
                              const void *addr = 0)
      : m_mfile(open_only, name, read_only, addr, 
                create_open_func_t(get_this_pointer(), 
                detail::DoOpen))
   {}

   //!Moves the ownership of "moved"'s managed memory to *this.
   //!Does not throw
   basic_managed_mapped_file(BOOST_INTERPROCESS_RV_REF(basic_managed_mapped_file) moved)
   {
      this->swap(moved);
   }

   //!Moves the ownership of "moved"'s managed memory to *this.
   //!Does not throw
   basic_managed_mapped_file &operator=(BOOST_INTERPROCESS_RV_REF(basic_managed_mapped_file) moved)
   {
      basic_managed_mapped_file tmp(boost::interprocess::move(moved));
      this->swap(tmp);
      return *this;
   }

   //!Destroys *this and indicates that the calling process is finished using
   //!the resource. The destructor function will deallocate
   //!any system resources allocated by the system for use by this process for
   //!this resource. The resource can still be opened again calling
   //!the open constructor overload. To erase the resource from the system
   //!use remove().
   ~basic_managed_mapped_file()
   {}

   //!Swaps the ownership of the managed mapped memories managed by *this and other.
   //!Never throws.
   void swap(basic_managed_mapped_file &other)
   {
      base_t::swap(other);
      m_mfile.swap(other.m_mfile);
   }

   //!Flushes cached data to file.
   //!Never throws
   bool flush()
   {  return m_mfile.flush();  }

   //!Tries to resize mapped file so that we have room for 
   //!more objects. 
   //!
   //!This function is not synchronized so no other thread or process should
   //!be reading or writing the file
   static bool grow(const char *filename, std::size_t extra_bytes)
   {
      return base_t::template grow
         <basic_managed_mapped_file>(filename, extra_bytes);
   }

   //!Tries to resize mapped file to minimized the size of the file.
   //!
   //!This function is not synchronized so no other thread or process should
   //!be reading or writing the file
   static bool shrink_to_fit(const char *filename)
   {
      return base_t::template shrink_to_fit
         <basic_managed_mapped_file>(filename);
   }

   /// @cond

   //!Tries to find a previous named allocation address. Returns a memory
   //!buffer and the object count. If not found returned pointer is 0.
   //!Never throws.
   template <class T>
   std::pair<T*, std::size_t> find  (char_ptr_holder_t name)
   {
      if(m_mfile.get_mapped_region().get_mode() == read_only){
         return base_t::template find_no_lock<T>(name);
      }
      else{
         return base_t::template find<T>(name);
      }
   }

   private:
   managed_open_or_create_type m_mfile;
   /// @endcond
};

}  //namespace interprocess {
}  //namespace boost {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_MANAGED_MAPPED_FILE_HPP
