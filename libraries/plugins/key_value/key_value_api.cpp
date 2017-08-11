#include <steemit/key_value/key_value_api.hpp>

namespace steemit {
    namespace key_value {

        namespace detail {

            class account_by_key_api_impl {
            public:
                account_by_key_api_impl(steemit::application::application &app)
                        : _app(app) {
                }

                vector<vector<account_name_type>> get_key_references(vector<public_key_type> &keys) const;

                steemit::application::application &_app;
            };

            vector<vector<account_name_type>> account_by_key_api_impl::get_key_references(vector<public_key_type> &keys) const {
                vector<vector<account_name_type>> final_result;
                final_result.reserve(keys.size());

                const auto &key_idx = _app.chain_database()->get_index<key_lookup_index>().indices().get<by_key>();

                for (auto &key : keys) {
                    vector<account_name_type> result;
                    auto lookup_itr = key_idx.lower_bound(key);

                    while (lookup_itr != key_idx.end() &&
                           lookup_itr->key == key) {
                        result.push_back(lookup_itr->account);
                        ++lookup_itr;
                    }

                    final_result.emplace_back(std::move(result));
                }

                return final_result;
            }

        } // detail

        account_by_key_api::account_by_key_api(const steemit::application::api_context &ctx) {
            my = std::make_shared<detail::account_by_key_api_impl>(ctx.app);
        }

        void account_by_key_api::on_api_startup() {
        }

    }
} // steemit::key_value