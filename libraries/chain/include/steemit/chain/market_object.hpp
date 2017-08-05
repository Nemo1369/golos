#pragma once

#include <steemit/protocol/asset.hpp>
#include <steemit/protocol/types.hpp>

namespace steemit {
    namespace chain {
        /**
         *  @brief an offer to sell a amount of a asset at a specified exchange rate by a certain time
         *  @ingroup object
         *  @ingroup protocol
         *  @ingroup market
         *
         *  This limit_order_objects are indexed by @ref expiration and is automatically deleted on the first block after expiration.
         */
        class limit_order_object : public object<limit_order_object_type, limit_order_object> {
        public:
            template<typename Constructor, typename Allocator>
            limit_order_object(Constructor &&c, allocator <Allocator> a) {
                c(*this);
            }

            limit_order_object() {

            }

            id_type id;

            time_point_sec created;
            time_point_sec expiration;
            account_name_type seller;
            protocol::integral_id_type order_id = 0;
            share_type for_sale; ///< asset id is sell_price.base.symbol
            protocol::price sell_price;
            share_type deferred_fee;

            pair <protocol::asset_name_type, protocol::asset_name_type> get_market() const {
                auto tmp = std::make_pair(sell_price.base.symbol_name(), sell_price.quote.symbol_name());
                if (tmp.first > tmp.second) {
                    std::swap(tmp.first, tmp.second);
                }
                return tmp;
            }

            protocol::asset amount_for_sale() const {
                return {for_sale, sell_price.base.symbol};
            }

            protocol::asset amount_to_receive() const {
                return amount_for_sale() * sell_price;
            }
        };

        /**
         * @class call_order_object
         * @brief tracks debt and call price information
         *
         * There should only be one call_order_object per asset pair per account and
         * they will all have the same call price.
         */
        class call_order_object : public object<call_order_object_type, call_order_object> {
        public:
            template<typename Constructor, typename Allocator>
            call_order_object(Constructor &&c, allocator <Allocator> a) {
                c(*this);
            }

            call_order_object() {

            }

            id_type id;

            protocol::asset get_collateral() const {
                return {collateral, call_price.base.symbol};
            }

            protocol::asset get_debt() const {
                return {debt, debt_type()};
            }

            protocol::asset amount_to_receive() const {
                return get_debt();
            }

            protocol::asset_name_type debt_type() const {
                return call_price.quote.symbol_name();
            }

            protocol::price collateralization() const {
                return get_collateral() / get_debt();
            }

            protocol::integral_id_type order_id;
            account_name_type borrower;
            share_type collateral;  ///< call_price.base.asset_id, access via get_collateral
            share_type debt;        ///< call_price.quote.asset_id, access via get_collateral
            protocol::price call_price;  ///< Debt / Collateral

            pair <protocol::asset_name_type, protocol::asset_name_type> get_market() const {
                auto tmp = std::make_pair(call_price.base.symbol_name(), call_price.quote.symbol_name());
                if (tmp.first > tmp.second) {
                    std::swap(tmp.first, tmp.second);
                }
                return tmp;
            }
        };

        /**
         *  @brief tracks bitassets scheduled for force settlement at some point in the future.
         *
         *  On the @ref settlement_date the @ref balance will be converted to the collateral asset
         *  and paid to @ref owner and then this object will be deleted.
         */
        class force_settlement_object : public object<force_settlement_object_type, force_settlement_object> {
        public:
            template<typename Constructor, typename Allocator>
            force_settlement_object(Constructor &&c, allocator <Allocator> a) {
                c(*this);
            }

            force_settlement_object() {

            }

            id_type id;

            account_name_type owner;
            protocol::integral_id_type settlement_id;
            protocol::asset balance;
            time_point_sec settlement_date;

            protocol::asset_name_type settlement_asset_symbol() const {
                return balance.symbol_name();
            }
        };

        struct by_price;
        struct by_expiration;
        struct by_account;
        typedef multi_index_container <limit_order_object, indexed_by<ordered_unique < tag <
                                                                      by_id>, member<limit_order_object, limit_order_object::id_type, &limit_order_object::id>>,
        ordered_non_unique <tag<by_expiration>, member<limit_order_object, time_point_sec, &limit_order_object::expiration>>,
        ordered_unique <tag<by_price>, composite_key<limit_order_object, member <
                                                                         limit_order_object, protocol::price, &limit_order_object::sell_price>, member<limit_order_object, limit_order_object::id_type, &limit_order_object::id>>,
        composite_key_compare <std::greater<protocol::price>, std::less<limit_order_object::id_type>>
        >,
        ordered_unique <tag<by_account>, composite_key<limit_order_object, member <
                                                                           limit_order_object, account_name_type, &limit_order_object::seller>, member<limit_order_object, protocol::integral_id_type, &limit_order_object::order_id>>
        >
        >,
        allocator <limit_order_object>
        >
        limit_order_index;

        struct by_collateral;
        struct by_account;
        struct by_price;
        typedef multi_index_container <call_order_object, indexed_by<ordered_unique < tag <
                                                                     by_id>, member<call_order_object, call_order_object::id_type, &call_order_object::id>>,
        ordered_unique <tag<by_price>, composite_key<call_order_object, member <
                                                                        call_order_object, protocol::price, &call_order_object::call_price>, member<call_order_object, protocol::integral_id_type, &call_order_object::order_id>>,
        composite_key_compare <std::less<protocol::price>, std::less<protocol::integral_id_type>>
        >,
        ordered_unique <tag<by_account>, composite_key<call_order_object, member <
                                                                          call_order_object, account_name_type, &call_order_object::borrower>, const_mem_fun<call_order_object, protocol::asset_name_type, &call_order_object::debt_type>>
        >,
        ordered_unique <tag<by_collateral>, composite_key<call_order_object, const_mem_fun <
                                                                             call_order_object, protocol::price, &call_order_object::collateralization>, member<call_order_object, protocol::integral_id_type, &call_order_object::order_id>>
        >
        >,allocator <call_order_object>
        >
        call_order_index;

        struct by_expiration;
        typedef multi_index_container <force_settlement_object, indexed_by<ordered_unique < tag <
                                                                           by_id>, member<force_settlement_object, force_settlement_object::id_type, &force_settlement_object::id>>,
        ordered_unique <tag<by_account>, composite_key<force_settlement_object, member <
                                                                                force_settlement_object, account_name_type, &force_settlement_object::owner>, member<force_settlement_object, protocol::integral_id_type, &force_settlement_object::settlement_id>>
        >,
        ordered_unique <tag<by_expiration>, composite_key<force_settlement_object, const_mem_fun <
                                                                                   force_settlement_object, protocol::asset_name_type, &force_settlement_object::settlement_asset_symbol>, member<force_settlement_object, time_point_sec, &force_settlement_object::settlement_date>, member<force_settlement_object, protocol::integral_id_type, &force_settlement_object::settlement_id>>
        >
        >,allocator <force_settlement_object>
        >
        force_settlement_index;
    }
} // steemit::chain

FC_REFLECT(steemit::chain::limit_order_object,
           (id)(created)(expiration)(seller)(order_id)(for_sale)(sell_price)(deferred_fee))
CHAINBASE_SET_INDEX_TYPE(steemit::chain::limit_order_object, steemit::chain::limit_order_index)

FC_REFLECT(steemit::chain::call_order_object, (id)(borrower)(collateral)(debt)(call_price))
CHAINBASE_SET_INDEX_TYPE(steemit::chain::call_order_object, steemit::chain::call_order_index)

FC_REFLECT(steemit::chain::force_settlement_object, (id)(owner)(balance)(settlement_date)(settlement_id))
CHAINBASE_SET_INDEX_TYPE(steemit::chain::force_settlement_object, steemit::chain::force_settlement_index)