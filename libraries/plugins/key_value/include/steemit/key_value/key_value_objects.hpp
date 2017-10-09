#pragma once

#include <steemit/chain/steem_object_types.hpp>

#include <steemit/protocol/base.hpp>

#include <boost/multi_index/composite_key.hpp>

namespace steemit {
    namespace key_value {

        using namespace std;
        using namespace steemit::chain;

#ifndef KEY_VALUE_STORE_SPACE_ID
#define KEY_VALUE_STORE_SPACE_ID 13
#endif

        enum key_value_object_types {
            first_key_value_object_type = (KEY_VALUE_STORE_SPACE_ID << 8),
            second_key_value_object_type = (KEY_VALUE_STORE_SPACE_ID << 9),
            third_key_value_object_type = (KEY_VALUE_STORE_SPACE_ID << 10)
        };

        class first_key_value_object
                : public object<first_key_value_object_type, first_key_value_object> {
        public:
            first_key_value_object() {

            }

            template<typename Constructor, typename Allocator>
            first_key_value_object(Constructor &&c, allocator<Allocator> a) {
                c(*this);
            }

            id_type id;

            string system;
            fc::uint128_t block_number;
            string block_hash;
            string ipfs_hash_link;
            fc::time_point_sec block_timestamp;
            fc::time_point_sec timestamp;
            account_name_type owner;

            protocol::extensions_type extensions;
        };

        class second_key_value_object
                : public object<second_key_value_object_type, second_key_value_object> {
        public:
            template<typename Constructor, typename Allocator>
            second_key_value_object(Constructor &&c, allocator<Allocator> a) {
                c(*this);
            }

            id_type id;

            string system;
            string address;
            fc::time_point_sec timestamp;
            account_name_type owner;

            protocol::extensions_type extensions;
        };

        using namespace boost::multi_index;

        struct by_system;
        struct by_block_number;
        struct by_block_hash;
        struct by_ipfs_hash;
        struct by_block_timestamp;
        struct by_timestamp;
        struct by_owner;
        struct by_address;

        typedef multi_index_container<
                first_key_value_object,
                indexed_by<
                        ordered_unique<tag<by_id>, member<first_key_value_object, first_key_value_object::id_type, &first_key_value_object::id>>,
                        ordered_non_unique<tag<by_system>, member<first_key_value_object, string, &first_key_value_object::system>>,
                        ordered_non_unique<tag<by_block_number>, member<first_key_value_object, fc::uint128_t, &first_key_value_object::block_number>>,
                        ordered_non_unique<tag<by_block_hash>, member<first_key_value_object, string, &first_key_value_object::block_hash>>,
                        ordered_non_unique<tag<by_ipfs_hash>, member<first_key_value_object, string, &first_key_value_object::ipfs_hash_link>>,
                        ordered_non_unique<tag<by_block_timestamp>, member<first_key_value_object, fc::time_point_sec, &first_key_value_object::block_timestamp>>,
                        ordered_non_unique<tag<by_timestamp>, member<first_key_value_object, fc::time_point_sec, &first_key_value_object::timestamp>>,
                        ordered_unique<tag<by_owner>, member<first_key_value_object, account_name_type, &first_key_value_object::owner>>
                >,
                allocator<first_key_value_object>
        > key_value_first_index;

        typedef multi_index_container<
                second_key_value_object,
                indexed_by<
                        ordered_unique<tag<by_id>, member<second_key_value_object, second_key_value_object::id_type, &second_key_value_object::id>>,
                        ordered_non_unique<tag<by_system>, member<second_key_value_object, string, &second_key_value_object::system>>,
                        ordered_non_unique<tag<by_address>, member<second_key_value_object, string, &second_key_value_object::address>>,
                        ordered_non_unique<tag<by_timestamp>, member<second_key_value_object, fc::time_point_sec, &second_key_value_object::timestamp>>,
                        ordered_unique<tag<by_owner>, member<second_key_value_object, account_name_type, &second_key_value_object::owner>>
                >,
                allocator<second_key_value_object>
        > key_value_second_index;

    }
} // steemit::key_value


FC_REFLECT(steemit::key_value::first_key_value_object, (id)(system)(block_number)(block_hash)(ipfs_hash_link)(block_timestamp)(timestamp)(owner))
CHAINBASE_SET_INDEX_TYPE(steemit::key_value::first_key_value_object, steemit::key_value::key_value_first_index)

FC_REFLECT(steemit::key_value::second_key_value_object, (id)(system)(address)(timestamp)(owner))
CHAINBASE_SET_INDEX_TYPE(steemit::key_value::second_key_value_object, steemit::key_value::key_value_second_index)